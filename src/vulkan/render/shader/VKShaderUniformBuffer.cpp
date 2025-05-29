//
// Created by grial on 14/11/22.
//

#include "VKShaderUniformBuffer.h"

#include <neon/render/shader/ShaderUniformDescriptor.h>
#include <vulkan/render/buffer/StagingBuffer.h>
#include <neon/render/textureold/Texture.h>
#include <cstring>
#include <utility>

namespace neon::vulkan
{
    VKShaderUniformBuffer::VKShaderUniformBuffer(const std::shared_ptr<ShaderUniformDescriptor>& descriptor) :
        _vkApplication(descriptor->getImplementation().getVkApplication()),
        _descriptorPool(VK_NULL_HANDLE)
    {
        _bindings = descriptor->getBindings();

        _buffers.reserve(_bindings.size());
        _data.reserve(_bindings.size());
        _updated.reserve(_bindings.size());

        _textures.resize(_bindings.size());

        uint32_t uniformAmount = 0;
        uint32_t storageAmount = 0;
        uint32_t textureAmount = 0;

        for (const auto& binding : _bindings) {
            _updated.emplace_back(_vkApplication->getMaxFramesInFlight(), false);

            switch (binding.type) {
                case UniformBindingType::UNIFORM_BUFFER: {
                    if (binding.bufferType == UniformBindingBufferType::STAGING) {
                        _buffers.push_back(std::make_shared<StagingBuffer>(
                            _vkApplication, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, binding.size));
                    } else {
                        _buffers.push_back(std::make_shared<SimpleBuffer>(
                            _vkApplication, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, binding.size));
                    }
                    _data.emplace_back(binding.size, 0);
                    ++uniformAmount;
                    break;
                }
                case UniformBindingType::STORAGE_BUFFER: {
                    if (binding.bufferType == UniformBindingBufferType::STAGING) {
                        _buffers.push_back(std::make_shared<StagingBuffer>(
                            _vkApplication, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, binding.size));
                    } else {
                        _buffers.push_back(std::make_shared<SimpleBuffer>(
                            _vkApplication, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, binding.size));
                    }
                    _data.emplace_back(binding.size, 0);
                    ++storageAmount;
                    break;
                }
                case UniformBindingType::IMAGE: {
                    _buffers.emplace_back();
                    _data.emplace_back();
                    ++textureAmount;
                    break;
                }
            }
        }

        std::vector<VkDescriptorPoolSize> pools;

        if (uniformAmount > 0) {
            pools.push_back(
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformAmount * _vkApplication->getMaxFramesInFlight()});
        }

        if (storageAmount > 0) {
            pools.push_back(
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, storageAmount * _vkApplication->getMaxFramesInFlight()});
        }

        if (textureAmount > 0) {
            pools.push_back(
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, textureAmount * _vkApplication->getMaxFramesInFlight()});
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(pools.size());
        poolInfo.pPoolSizes = pools.data();
        poolInfo.maxSets = _vkApplication->getMaxFramesInFlight();

        if (vkCreateDescriptorPool(_vkApplication->getDevice()->getRaw(), &poolInfo, nullptr, &_descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }

        std::vector<VkDescriptorSetLayout> layouts(_vkApplication->getMaxFramesInFlight(),
                                                   descriptor->getImplementation().getDescriptorSetLayout());

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.descriptorSetCount = _vkApplication->getMaxFramesInFlight();
        allocInfo.pSetLayouts = layouts.data();

        _descriptorSets.resize(_vkApplication->getMaxFramesInFlight());
        if (vkAllocateDescriptorSets(_vkApplication->getDevice()->getRaw(), &allocInfo, _descriptorSets.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }

        for (size_t frame = 0; frame < _vkApplication->getMaxFramesInFlight(); ++frame) {
            for (int bindingIndex = 0; bindingIndex < _bindings.size(); ++bindingIndex) {
                auto& binding = _bindings[bindingIndex];

                if (binding.type == UniformBindingType::UNIFORM_BUFFER ||
                    binding.type == UniformBindingType::STORAGE_BUFFER) {
                    VkDescriptorBufferInfo bufferInfo{};
                    bufferInfo.buffer = _buffers[bindingIndex]->getRaw(nullptr);
                    bufferInfo.offset = 0;
                    bufferInfo.range = binding.size;

                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = _descriptorSets[frame];
                    descriptorWrite.dstBinding = bindingIndex;
                    descriptorWrite.dstArrayElement = 0;

                    descriptorWrite.descriptorType = binding.type == UniformBindingType::UNIFORM_BUFFER
                                                         ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
                                                         : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pBufferInfo = &bufferInfo;

                    vkUpdateDescriptorSets(_vkApplication->getDevice()->getRaw(), 1, &descriptorWrite, 0, nullptr);
                }
            }
        }
    }

    VKShaderUniformBuffer::~VKShaderUniformBuffer()
    {
        _vkApplication->getBin()->destroyLater(_vkApplication->getDevice()->getRaw(), getRuns(), _descriptorPool,
                                               vkDestroyDescriptorPool);
    }

    void VKShaderUniformBuffer::uploadData(uint32_t index, const void* data, size_t size, size_t offset)
    {
        if (index >= _bindings.size()) {
            return;
        }
        if (_bindings[index].type != UniformBindingType::UNIFORM_BUFFER &&
            _bindings[index].type != UniformBindingType::STORAGE_BUFFER) {
            return;
        }
        auto& vector = _data[index];
        auto finalSize = std::min(size, vector.size());
        memcpy(vector.data() + offset, data, finalSize);
        std::ranges::fill(_updated[index], 0);
    }

    void VKShaderUniformBuffer::clearData(uint32_t index)
    {
        if (index >= _bindings.size()) {
            return;
        }
        if (_bindings[index].type != UniformBindingType::UNIFORM_BUFFER &&
            _bindings[index].type != UniformBindingType::STORAGE_BUFFER) {
            return;
        }
        std::ranges::fill(_updated[index], 0);
        std::ranges::fill(_data[index], 0);
    }

    void* VKShaderUniformBuffer::fetchData(uint32_t index)
    {
        if (index >= _bindings.size()) {
            return nullptr;
        }
        if (_bindings[index].type != UniformBindingType::UNIFORM_BUFFER &&
            _bindings[index].type != UniformBindingType::STORAGE_BUFFER) {
            return nullptr;
        }
        std::ranges::fill(_updated[index], 0);
        return _data[index].data();
    }

    const void* VKShaderUniformBuffer::fetchData(uint32_t index) const
    {
        if (index >= _bindings.size()) {
            return nullptr;
        }
        if (_bindings[index].type != UniformBindingType::UNIFORM_BUFFER &&
            _bindings[index].type != UniformBindingType::STORAGE_BUFFER) {
            return nullptr;
        }
        return _data[index].data();
    }

    void VKShaderUniformBuffer::setTexture(uint32_t index, std::shared_ptr<Texture> texture)
    {
        if (index >= _bindings.size()) {
            return;
        }
        if (_bindings[index].type != UniformBindingType::IMAGE) {
            return;
        }
        _textures[index] = std::move(texture);
        std::ranges::fill(_updated[index], 0);
    }

    void VKShaderUniformBuffer::prepareForFrame(const CommandBuffer* commandBuffer)
    {
        uint32_t frame = _vkApplication->getCurrentFrame();
        for (int index = 0; index < _updated.size(); ++index) {
            auto& updated = _updated[index];

            switch (_bindings[index].type) {
                case UniformBindingType::STORAGE_BUFFER:
                case UniformBindingType::UNIFORM_BUFFER: {
                    if (updated[frame] == 1) {
                        continue;
                    }
                    std::fill(updated.begin(), updated.end(), 1);

                    auto optional = _buffers[index]->map<char>(commandBuffer);
                    if (optional.has_value()) {
                        auto& data = _data[index];
                        memcpy(optional.value()->raw(), data.data(), data.size());
                    } else {
                        logger.error("Optional has no value.");
                    }
                } break;
                case UniformBindingType::IMAGE: {
                    auto texture = _textures[index];
                    if (texture == nullptr) {
                        continue;
                    }
                    auto flag = texture->getImplementation().getExternalDirtyFlag();
                    if (updated[frame] == flag) {
                        continue;
                    }
                    updated[frame] = flag;

                    VkDescriptorImageInfo imageInfo{};

                    if (texture != nullptr) {
                        auto& impl = texture->getImplementation();
                        imageInfo.imageView = impl.getImageView();
                        imageInfo.sampler = impl.getSampler();
                        imageInfo.imageLayout = impl.getLayout();
                    } else {
                        imageInfo.imageView = VK_NULL_HANDLE;
                        imageInfo.sampler = VK_NULL_HANDLE;
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    }

                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = _descriptorSets[frame];
                    descriptorWrite.dstBinding = index;
                    descriptorWrite.dstArrayElement = 0;

                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pImageInfo = &imageInfo;

                    vkUpdateDescriptorSets(_vkApplication->getDevice()->getRaw(), 1, &descriptorWrite, 0, nullptr);
                } break;
            }
        }
    }

    void VKShaderUniformBuffer::transferDataFromGPU(uint32_t index)
    {
        if (index >= _bindings.size()) {
            return;
        }
        auto& binding = _bindings[index];
        if (binding.type != UniformBindingType::STORAGE_BUFFER && binding.type != UniformBindingType::UNIFORM_BUFFER) {
            return;
        }

        auto& buffer = _buffers[index];
        std::optional<std::shared_ptr<BufferMap<char>>> optional;
        if (binding.bufferType == UniformBindingBufferType::SIMPLE) {
            // Simple map.
            optional = buffer->map<char>();
        } else {
            return;
        }

        auto& data = _data[index];
        memcpy(data.data(), optional.value()->raw(), data.size());
    }

    void VKShaderUniformBuffer::bind(VKCommandBuffer* commandBuffer, VkPipelineLayout layout, uint32_t bindingPoint)
    {
        registerRun(commandBuffer->getCurrentRun());
        for (auto& texture : _textures) {
            if (texture != nullptr) {
                texture->getImplementation().registerRun(commandBuffer->getCurrentRun());
            }
        }
        vkCmdBindDescriptorSets(commandBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, layout,
                                bindingPoint, 1, &_descriptorSets[_vkApplication->getCurrentFrame()], 0, nullptr);
    }
} // namespace neon::vulkan
