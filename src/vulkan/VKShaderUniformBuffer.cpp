//
// Created by grial on 14/11/22.
//

#include "VKShaderUniformBuffer.h"

#include <engine/Application.h>
#include <vulkan/buffer/SimpleBuffer.h>

VKShaderUniformBuffer::VKShaderUniformBuffer(
        Application* application, uint32_t size) :
        _vkApplication(&application->getImplementation()),
        _descriptorSetLayout(VK_NULL_HANDLE),
        _descriptorPool(VK_NULL_HANDLE),
        _buffers(),
        _descriptorSets(),
        _updated(),
        _currentImage(0),
        _maxSize(static_cast<size_t>(size)),
        _data(),
        _bindingPoint(0) {

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(
            _vkApplication->getDevice(), &layoutInfo,
            nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }

    _buffers.reserve(_vkApplication->getMaxFramesInFlight());
    for (int i = 0; i < _vkApplication->getMaxFramesInFlight(); ++i) {
        _buffers.push_back(std::make_shared<SimpleBuffer>(
                _vkApplication->getPhysicalDevice(),
                _vkApplication->getDevice(),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                size));
    }

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = _vkApplication->getMaxFramesInFlight();

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = _vkApplication->getMaxFramesInFlight();

    if (vkCreateDescriptorPool(
            _vkApplication->getDevice(), &poolInfo, nullptr,
            &_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }

    std::vector<VkDescriptorSetLayout> layouts(
            _vkApplication->getMaxFramesInFlight(),
            _descriptorSetLayout
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

    for (int i = 0; i < _vkApplication->getMaxFramesInFlight(); ++i) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = _buffers[i]->getRaw();
        bufferInfo.offset = 0;
        bufferInfo.range = size;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = _descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;

        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(_vkApplication->getDevice(),
                               1, &descriptorWrite, 0, nullptr);
    }

    _updated.resize(_vkApplication->getMaxFramesInFlight());
}

VKShaderUniformBuffer::~VKShaderUniformBuffer() {
    vkDestroyDescriptorPool(_vkApplication->getDevice(),
                            _descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(_vkApplication->getDevice(),
                                 _descriptorSetLayout, nullptr);
}

void VKShaderUniformBuffer::setBindingPoint(uint32_t point) const {
    _bindingPoint = point;
}

void VKShaderUniformBuffer::uploadData(const void* data, size_t size) {
    _data.resize(std::min(_maxSize, size));
    memcpy(_data.data(), data, size);
    std::fill(_updated.begin(), _updated.end(), false);
}

void VKShaderUniformBuffer::prepareForFrame() {
    uint32_t frame = _vkApplication->getCurrentFrame();
    if (_updated[frame]) return;
    _updated[frame] = true;

    auto optional = _buffers[frame]->map<char>();
    if (optional.has_value()) {
        memcpy(optional.value()->raw(), _data.data(), _data.size());
    }
}

VkDescriptorSetLayout VKShaderUniformBuffer::getDescriptorSetLayout() const {
    return _descriptorSetLayout;
}

void VKShaderUniformBuffer::bind(VkCommandBuffer commandBuffer,
                                 VkPipelineLayout layout) const {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout, _bindingPoint, 1,
                            &_descriptorSets[_currentImage], 0, nullptr);
}
