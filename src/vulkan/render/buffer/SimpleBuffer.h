//
// Created by gaelr on 14/10/2022.
//

#ifndef VULKANTEST_SIMPLEBUFFER_H
#define VULKANTEST_SIMPLEBUFFER_H

#include <vector>
#include <stdexcept>
#include <string>

#include <vulkan/render/buffer/Buffer.h>

namespace neon::vulkan::simple_buffer {
    VkResult mapMemory(VkDevice device, VkDeviceMemory memory, uint32_t from,
                       uint32_t size, int flags, void** destiny);

    void unmapMemory(VkDevice device, VkDeviceMemory memory);
}

namespace neon::vulkan {
    class AbstractVKApplication;

    template<class T>
    class SimpleBufferMap : public BufferMap<T> {
        VkDevice _device;
        VkDeviceMemory _memory;

        T* _pointer;

    public:
        SimpleBufferMap(const SimpleBufferMap& other) = delete;

        SimpleBufferMap(SimpleBufferMap&& other) noexcept :
            _device(other._device),
            _memory(other._memory),
            _pointer(other._pointer) {
            other._pointer = nullptr;
        }

        SimpleBufferMap() = default;

        SimpleBufferMap(VkDevice device, VkDeviceMemory memory,
                        Range<uint32_t> range) :
            BufferMap<T>(),
            _device(device),
            _memory(memory),
            _pointer(nullptr) {
            auto result = simple_buffer::mapMemory(
                device, memory, range.getFrom(), range.size(),
                0, (void**)&_pointer);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Couldn't map buffer! Result: "
                                         + std::to_string(result));
            }
        }

        ~SimpleBufferMap() override {
            if (_pointer == nullptr) return;
            simple_buffer::unmapMemory(_device, _memory);
            _pointer = nullptr;
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
            simple_buffer::unmapMemory(_device, _memory);
            _pointer = nullptr;
        }

        bool isValid() override {
            return _pointer != nullptr;
        }

        SimpleBufferMap& operator=(SimpleBufferMap&& other) noexcept {
            _device = other._device;
            _memory = other._memory;
            _pointer = other._pointer;
            other._pointer = nullptr;
            return *this;
        }
    };

    class SimpleBuffer : public Buffer {
        size_t _size;

        AbstractVKApplication* _application;
        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertexBufferMemory;

        bool _modifiable;

        std::optional<std::shared_ptr<BufferMap<char>>> rawMap(
            const CommandBuffer* commandBuffer = nullptr) override;

        std::optional<std::shared_ptr<BufferMap<char>>> rawMap(
            Range<uint32_t> range,
            const CommandBuffer* commandBuffer = nullptr) override;

    public:
        SimpleBuffer(const SimpleBuffer& other) = delete;

        template<class T>
        SimpleBuffer(AbstractVKApplication* application,
                     VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                     const std::vector<T>& data):
            SimpleBuffer(application, usage, properties, data.data(),
                         static_cast<uint32_t>(data.size() * sizeof(T))) {}

        SimpleBuffer(AbstractVKApplication* application,
                     VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                     uint32_t sizeInBytes);

        SimpleBuffer(AbstractVKApplication* application,
                     VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                     const void* data, uint32_t sizeInBytes);

        ~SimpleBuffer() override;

        [[nodiscard]] size_t size() const override;

        [[nodiscard]] bool canBeWrittenOn() const override;

        [[nodiscard]] VkBuffer getRaw() const override;

        [[nodiscard]] AbstractVKApplication* getApplication() const override;
    };
}

#endif //VULKANTEST_SIMPLEBUFFER_H
