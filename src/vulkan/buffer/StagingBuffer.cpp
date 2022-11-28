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

std::optional<std::shared_ptr<BufferMap<char>>> StagingBuffer::rawMap() {
    return std::make_shared<StagingBufferMap<char>>(
            _application, _stagingBuffer, _deviceBuffer);
}

std::optional<std::shared_ptr<BufferMap<char>>>
StagingBuffer::rawMap(Range<uint32_t> range) {
    return std::make_shared<StagingBufferMap<char>>(
            _application, _stagingBuffer, _deviceBuffer, range);
}


StagingBuffer::StagingBuffer(VKApplication* application,
                             VkBufferUsageFlags usage, uint32_t sizeInBytes) :
        _application(application),
        _stagingBuffer(_application, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeInBytes),
        _deviceBuffer(_application,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sizeInBytes) {
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

VKApplication* StagingBuffer::getApplication() const {
    return _application;
}
