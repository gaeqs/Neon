//
// Created by grial on 17/10/22.
//

#ifndef VULKANTEST_STAGINGBUFFER_H
#define VULKANTEST_STAGINGBUFFER_H


#include <vector>
#include "Buffer.h"
#include "SimpleBuffer.h"

#include <vulkan/util/VKUtil.h>

class StagingBuffer;

template<class T>
class StagingBufferMap : public BufferMap<T> {

    VkQueue _queue;
    VkCommandPool _commandPool;

    SimpleBuffer& _stagingBuffer;
    SimpleBuffer& _deviceBuffer;

    std::shared_ptr<BufferMap<T>> _map;

public:

    StagingBufferMap(VkQueue queue, VkCommandPool commandPool,
                     SimpleBuffer& stagingBuffer,
                     SimpleBuffer& deviceBuffer) :
            BufferMap<T>(),
            _queue(queue),
            _commandPool(commandPool),
            _stagingBuffer(stagingBuffer),
            _deviceBuffer(deviceBuffer),
            _map(_stagingBuffer.map<T>().value()) {
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
                _deviceBuffer.getDevice(),
                _commandPool,
                _queue,
                _stagingBuffer.getRaw(),
                _deviceBuffer.getRaw(),
                _deviceBuffer.size()
        );
    }

};

class StagingBuffer : public Buffer {

    SimpleBuffer _stagingBuffer;
    SimpleBuffer _deviceBuffer;
    VkCommandPool _commandPool;
    VkQueue _queue;

    std::optional<std::shared_ptr<BufferMap<char>>> rawMap() override;

public:

    template<class T>
    StagingBuffer(VkQueue queue, VkCommandPool commandPool,
                  VkPhysicalDevice physicalDevice, VkDevice device,
                  VkBufferUsageFlags usage, const std::vector<T>& data) :
            _stagingBuffer(physicalDevice, device,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, data),
            _deviceBuffer(physicalDevice, device,
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, data),
            _commandPool(commandPool),
            _queue(queue) {
        map<T>(); // Transfers all data from the staging buffer to the device buffer.
    }

    StagingBuffer(VkQueue queue, VkCommandPool commandPool,
                  VkPhysicalDevice physicalDevice, VkDevice device,
                  VkBufferUsageFlags usage, uint32_t sizeInBytes);

    ~StagingBuffer() override = default;

    size_t size() const override;

    bool canBeWrittenOn() const override;

    VkBuffer getRaw() const override;

    VkPhysicalDevice getPhysicalDevice() const override;

    VkDevice getDevice() const override;

    SimpleBuffer& getStagingBuffer();

    const SimpleBuffer& getStagingBuffer() const;

    SimpleBuffer& getDeviceBuffer();

    const SimpleBuffer& getDeviceBuffer() const;

    VkCommandPool getCommandPool() const;

};


#endif //VULKANTEST_STAGINGBUFFER_H
