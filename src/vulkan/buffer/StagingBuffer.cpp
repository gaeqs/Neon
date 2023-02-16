//
// Created by grial on 17/10/22.
//

#include "StagingBuffer.h"

namespace neon::vulkan {
    size_t StagingBuffer::size() const {
        return _deviceBuffer.size();
    }

    bool StagingBuffer::canBeWrittenOn() const {
        return true;
    }

    VkBuffer StagingBuffer::getRaw() const {
        return _deviceBuffer.getRaw();
    }

    std::optional<std::shared_ptr<BufferMap<char>>> StagingBuffer::rawMap() {
        auto& buffer = _stagingBuffers[_application->getCurrentFrame()];
        return std::make_shared<StagingBufferMap<char>>(
                _application, buffer, _deviceBuffer);
    }

    std::optional<std::shared_ptr<BufferMap<char>>>
    StagingBuffer::rawMap(Range<uint32_t> range) {
        auto& buffer = _stagingBuffers[_application->getCurrentFrame()];
        return std::make_shared<StagingBufferMap<char>>(
                _application, buffer, _deviceBuffer, range);
    }


    StagingBuffer::StagingBuffer(VKApplication* application,
                                 VkBufferUsageFlags usage,
                                 uint32_t sizeInBytes) :
            _application(application),
            _stagingBuffers(),
            _deviceBuffer(_application,
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sizeInBytes) {
        for (int i = 0; i < _application->getMaxFramesInFlight(); ++i) {
            _stagingBuffers.push_back(std::make_shared<SimpleBuffer>(
                    _application,
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    sizeInBytes
            ));
        }

    }

    SimpleBuffer& StagingBuffer::getDeviceBuffer() {
        return _deviceBuffer;
    }

    const SimpleBuffer& StagingBuffer::getDeviceBuffer() const {
        return _deviceBuffer;
    }

    VKApplication* StagingBuffer::getApplication() const {
        return _application;
    }
}
