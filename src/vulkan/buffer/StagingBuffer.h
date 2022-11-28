//
// Created by grial on 17/10/22.
//

#ifndef VULKANTEST_STAGINGBUFFER_H
#define VULKANTEST_STAGINGBUFFER_H


#include <vector>
#include "Buffer.h"
#include "SimpleBuffer.h"

#include <vulkan/util/VKUtil.h>
#include <vulkan/buffer/SimpleBuffer.h>

class StagingBuffer;

template<class T>
class StagingBufferMap : public BufferMap<T> {

    VKApplication* _application;

    SimpleBuffer& _stagingBuffer;
    SimpleBuffer& _deviceBuffer;

    std::shared_ptr<BufferMap<T>> _map;

public:

    StagingBufferMap(VKApplication* application,
                     SimpleBuffer& stagingBuffer,
                     SimpleBuffer& deviceBuffer) :
            BufferMap<T>(),
            _application(application),
            _stagingBuffer(stagingBuffer),
            _deviceBuffer(deviceBuffer),
            _map(_stagingBuffer.map<T>().value()) {
    }

    StagingBufferMap(VKApplication* application,
                     SimpleBuffer& stagingBuffer,
                     SimpleBuffer& deviceBuffer,
                     Range<uint32_t> range) :
            BufferMap<T>(),
            _application(application),
            _stagingBuffer(stagingBuffer),
            _deviceBuffer(deviceBuffer),
            _map(_stagingBuffer.map<T>(range).value()) {
    }

    ~StagingBufferMap() override {
        dispose();
    };

    T& operator[](size_t index) override {
        return _map->raw()[index];
    }

    T operator[](size_t index) const override {
        return _map->raw()[index];
    }

    T* raw() override {
        return _map->raw();
    }

    void dispose() override {
        if (_map->raw() == nullptr) return;
        _map->dispose();
        vulkan_util::copyBuffer(
                _application,
                _stagingBuffer.getRaw(),
                _deviceBuffer.getRaw(),
                _deviceBuffer.size()
        );
    }

};

class StagingBuffer : public Buffer {

    VKApplication* _application;

    SimpleBuffer _stagingBuffer;
    SimpleBuffer _deviceBuffer;

    std::optional<std::shared_ptr<BufferMap<char>>> rawMap() override;

    std::optional<std::shared_ptr<BufferMap<char>>> rawMap(
            Range<uint32_t> range) override;

public:

    template<class T>
    StagingBuffer(VKApplication* application,
                  VkBufferUsageFlags usage, const std::vector<T>& data) :
            _application(application),
            _stagingBuffer(_application,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, data),
            _deviceBuffer(_application,
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, data) {
        map<T>(); // Transfers all data from the staging buffer to the device buffer.
    }

    StagingBuffer(VKApplication* application,
                  VkBufferUsageFlags usage, uint32_t sizeInBytes);

    ~StagingBuffer() override = default;

    size_t size() const override;

    bool canBeWrittenOn() const override;

    VkBuffer getRaw() const override;

    VKApplication* getApplication() const override;

    SimpleBuffer& getStagingBuffer();

    const SimpleBuffer& getStagingBuffer() const;

    SimpleBuffer& getDeviceBuffer();

    const SimpleBuffer& getDeviceBuffer() const;

};


#endif //VULKANTEST_STAGINGBUFFER_H
