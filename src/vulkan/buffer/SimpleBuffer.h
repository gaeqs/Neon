//
// Created by gaelr on 14/10/2022.
//

#ifndef VULKANTEST_SIMPLEBUFFER_H
#define VULKANTEST_SIMPLEBUFFER_H

#include <vector>
#include <stdexcept>
#include "Buffer.h"

template<class T>
class SimpleBufferMap : public BufferMap<T> {

    VkDevice _device;
    VkDeviceMemory _memory;

    T* _pointer;

public:

    SimpleBufferMap(VkDevice device, VkDeviceMemory memory, size_t size) :
            BufferMap<T>(),
            _device(device),
            _memory(memory),
            _pointer(nullptr) {
        vkMapMemory(device, memory, 0, size, 0, (void**) &_pointer);
    }

    ~SimpleBufferMap() override {
        dispose();
    };

    T& operator[](size_t index) override {
        return _pointer[index];
    }

    T operator[](size_t index) const override {
        return _pointer[index];
    }

    T* raw() override {
        return _pointer;
    }

    void dispose() override {
        if (_pointer == nullptr) return;
        vkUnmapMemory(_device, _memory);
        _pointer = nullptr;
    }

};

class SimpleBuffer : public Buffer {

    size_t _size;

    VkPhysicalDevice _physicalDevice;
    VkDevice _device;
    VkBuffer _vertexBuffer;
    VkDeviceMemory _vertexBufferMemory;

    bool _modifiable;

    std::optional<std::shared_ptr<BufferMap<char>>> rawMap() override;

public:

    SimpleBuffer(const SimpleBuffer& other) = delete;

    template<class T>
    SimpleBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                 VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                 const std::vector<T>& data):
            SimpleBuffer(physicalDevice, device, usage, properties, data.data(),
                         data.size() * sizeof(T)) {
    }

    SimpleBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                 VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                 uint32_t sizeInBytes);

    SimpleBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                 VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                 const void* data, uint32_t sizeInBytes);

    ~SimpleBuffer() override;

    [[nodiscard]] size_t size() const override;

    [[nodiscard]] bool canBeWrittenOn() const override;

    [[nodiscard]] VkBuffer getRaw() const override;

    [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const override;

    [[nodiscard]] VkDevice getDevice() const override;
};


#endif //VULKANTEST_SIMPLEBUFFER_H
