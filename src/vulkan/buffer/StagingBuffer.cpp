//
// Created by grial on 17/10/22.
//

#include "StagingBuffer.h"

size_t StagingBuffer::size() const {
    return _stagingBuffer.size();
}

bool StagingBuffer::canBeWrittenOn() const {
    return true;
}

VkBuffer StagingBuffer::getRaw() const {
    return _deviceBuffer.getRaw();
}

VkPhysicalDevice StagingBuffer::getPhysicalDevice() const {
    return _stagingBuffer.getPhysicalDevice();
}

VkDevice StagingBuffer::getDevice() const {
    return _stagingBuffer.getDevice();
}

std::optional<std::shared_ptr<BufferMap<char>>> StagingBuffer::rawMap() {
    return std::make_shared<StagingBufferMap<char>>(_queue, _commandPool,
                                                    _stagingBuffer,
                                                    _deviceBuffer);
}

StagingBuffer::StagingBuffer(VkQueue queue, VkCommandPool commandPool,
                             VkPhysicalDevice physicalDevice, VkDevice device,
                             VkBufferUsageFlags usage, uint32_t sizeInBytes) :
        _stagingBuffer(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeInBytes),
        _deviceBuffer(physicalDevice, device,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sizeInBytes),
        _commandPool(commandPool),
        _queue(queue) {
}


SimpleBuffer& StagingBuffer::getStagingBuffer() {
    return _stagingBuffer;
}

SimpleBuffer& StagingBuffer::getDeviceBuffer() {
    return _deviceBuffer;
}

const SimpleBuffer& StagingBuffer::getStagingBuffer() const {
    return _stagingBuffer;
}

const SimpleBuffer& StagingBuffer::getDeviceBuffer() const {
    return _deviceBuffer;
}

VkCommandPool StagingBuffer::getCommandPool() const {
    return _commandPool;
}
