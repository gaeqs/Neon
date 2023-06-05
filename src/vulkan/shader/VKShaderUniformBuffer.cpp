//
// Created by grial on 14/11/22.
//

#include "VKShaderUniformBuffer.h"

#include <engine/shader/ShaderUniformDescriptor.h>
#include <vulkan/buffer/StagingBuffer.h>
#include <engine/render/Texture.h>
#include <cstring>
#include <utility>

namespace neon::vulkan {
    VKShaderUniformBuffer::VKShaderUniformBuffer(
            const std::shared_ptr<ShaderUniformDescriptor>& descriptor) :
            _vkApplication(descriptor->getImplementation().getVkApplication()),
            _descriptorPool(VK_NULL_HANDLE),
            _buffers(),
            _descriptorSets(),
            _types(),
            _updated(),
            _data(),
            _textures(),
            _bindingPoint(0) {

        auto& bindings = descriptor->getBindings();

        _buffers.reserve(bindings.size());
        _types.reserve(bindings.size());
        _data.reserve(bindings.size());
        _updated.reserve(bindings.size());

        _textures.resize(bindings.size());

        uint32_t bufferAmount = 0;
        uint32_t textureAmount = 0;

        for (const auto& binding: bindings) {
            _updated.emplace_back(_vkApplication->getMaxFramesInFlight(),
                                  false);
            _types.emplace_back(binding.type);

            if (binding.type == UniformBindingType::IMAGE) {
                _buffers.emplace_back();
                _data.emplace_back();
                ++textureAmount;
            } else {
                std::vector<std::shared_ptr<Buffer>> buffers;
                _buffers.push_back(std::make_shared<StagingBuffer>(
                        _vkApplication,
                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        binding.size));
                _data.emplace_back(binding.size, 0);
                ++bufferAmount;
            }
        }

        std::vector<VkDescriptorPoolSize> pools;

        if (bufferAmount > 0) {
            pools.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                             bufferAmount *
                             _vkApplication->getMaxFramesInFlight()});
        }
        if (textureAmount > 0) {
            pools.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                             textureAmount *
                             _vkApplication->getMaxFramesInFlight()});
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = pools.size();
        poolInfo.pPoolSizes = pools.data();
        poolInfo.maxSets = _vkApplication->getMaxFramesInFlight();

        if (vkCreateDescriptorPool(
                _vkApplication->getDevice(), &poolInfo, nullptr,
                &_descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }

        std::vector<VkDescriptorSetLayout> layouts(
                _vkApplication->getMaxFramesInFlight(),
                descriptor->getImplementation().getDescriptorSetLayout()
        );

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.descriptorSetCount = _vkApplication->getMaxFramesInFlight();
        allocInfo.pSetLayouts = layouts.data();

        _descriptorSets.resize(_vkApplication->getMaxFramesInFlight());
        if (vkAllocateDescriptorSets(
                _vkApplication->getDevice(), &allocInfo,
                _descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }

        for (int frame = 0; frame < _vkApplication->getMaxFramesInFlight();
             ++frame) {
            for (int bindingIndex = 0; bindingIndex < bindings.size();
                 ++bindingIndex) {

                auto& binding = bindings[bindingIndex];

                if (binding.type == UniformBindingType::BUFFER) {
                    VkDescriptorBufferInfo bufferInfo{};
                    bufferInfo.buffer = _buffers[bindingIndex]->getRaw();
                    bufferInfo.offset = 0;
                    bufferInfo.range = binding.size;

                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = _descriptorSets[frame];
                    descriptorWrite.dstBinding = bindingIndex;
                    descriptorWrite.dstArrayElement = 0;

                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pBufferInfo = &bufferInfo;

                    vkUpdateDescriptorSets(_vkApplication->getDevice(),
                                           1, &descriptorWrite, 0, nullptr);
                }
            }


        }
    }

    VKShaderUniformBuffer::~VKShaderUniformBuffer() {
        vkDestroyDescriptorPool(_vkApplication->getDevice(),
                                _descriptorPool, nullptr);
    }

    void VKShaderUniformBuffer::setBindingPoint(uint32_t point) {
        _bindingPoint = point;
    }

    void VKShaderUniformBuffer::uploadData(uint32_t index,
                                           const void* data,
                                           size_t size) {
        if (index >= _types.size()) return;
        if (_types[index] != UniformBindingType::BUFFER) return;
        auto& vector = _data[index];
        auto finalSize = std::min(size, vector.size());
        memcpy(vector.data(), data, finalSize);
        std::fill(_updated[index].begin(), _updated[index].end(), 0);
    }

    void VKShaderUniformBuffer::setTexture(
            uint32_t index,
            std::shared_ptr<Texture> texture) {
        if (index >= _types.size()) return;
        if (_types[index] != UniformBindingType::IMAGE) return;
        _textures[index] = std::move(texture);
        std::fill(_updated[index].begin(), _updated[index].end(), 0);
    }

    void VKShaderUniformBuffer::prepareForFrame(
            const CommandBuffer* commandBuffer) {
        uint32_t frame = _vkApplication->getCurrentFrame();
        for (int index = 0; index < _updated.size(); ++index) {
            auto& updated = _updated[index];

            switch (_types[index]) {
                case UniformBindingType::BUFFER: {
                    if (updated[frame] == 1) continue;
                    std::fill(updated.begin(), updated.end(), 1);

                    auto optional = _buffers[index]->map<char>(commandBuffer);
                    if (optional.has_value()) {
                        auto& data = _data[index];
                        memcpy(optional.value()->raw(), data.data(),
                               data.size());
                    }
                }
                    break;
                case UniformBindingType::IMAGE: {
                    auto texture = _textures[index];
                    if (texture == nullptr) continue;
                    auto flag = texture->getImplementation().getExternalDirtyFlag();
                    if (updated[frame] == flag) continue;
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
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
                    }

                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = _descriptorSets[frame];
                    descriptorWrite.dstBinding = index;
                    descriptorWrite.dstArrayElement = 0;

                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pImageInfo = &imageInfo;

                    vkUpdateDescriptorSets(_vkApplication->getDevice(),
                                           1, &descriptorWrite, 0, nullptr);
                }
                    break;
            }


        }
    }

    void VKShaderUniformBuffer::bind(VkCommandBuffer commandBuffer,
                                     VkPipelineLayout layout) const {
        vkCmdBindDescriptorSets(
                commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                layout, _bindingPoint, 1,
                &_descriptorSets[_vkApplication->getCurrentFrame()],
                0, nullptr);
    }
}