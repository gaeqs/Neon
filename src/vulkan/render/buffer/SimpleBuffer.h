//
// Created by gaelr on 14/10/2022.
//

#ifndef VULKANTEST_SIMPLEBUFFER_H
#define VULKANTEST_SIMPLEBUFFER_H

#include <vector>
#include <stdexcept>
#include <string>
#include <vma/vk_mem_alloc.h>

#include <vulkan/render/buffer/Buffer.h>

namespace neon::vulkan::simple_buffer
{
    VkResult mapMemory(VkDevice device, VkDeviceMemory memory, uint32_t from, uint32_t size, int flags, void** destiny);

    void unmapMemory(VkDevice device, VkDeviceMemory memory);
} // namespace neon::vulkan::simple_buffer

namespace neon::vulkan
{
    class AbstractVKApplication;

    template<class T>
    class SimpleBufferMap : public BufferMap<T>
    {
        VmaAllocator _allocator;
        VmaAllocation _allocation;

        T* _pointer;

      public:
        SimpleBufferMap(const SimpleBufferMap& other) = delete;

        SimpleBufferMap(SimpleBufferMap&& other) noexcept :
            _allocator(other._allocator),
            _allocation(other._allocation),
            _pointer(other._pointer)
        {
            other._pointer = nullptr;
        }

        SimpleBufferMap() = default;

        SimpleBufferMap(VmaAllocator allocator, VmaAllocation allocation, Range<uint32_t> range) :
            BufferMap<T>(),
            _allocator(allocator),
            _allocation(allocation),
            _pointer(nullptr)
        {
            void* ptr;
            auto result = vmaMapMemory(_allocator, _allocation, &ptr);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Couldn't map buffer! Result: " + std::to_string(result));
            }

            // Add offset
            _pointer = static_cast<T*>(static_cast<void*>(static_cast<std::byte*>(ptr) + range.getFrom()));
        }

        ~SimpleBufferMap() override
        {
            if (_pointer == nullptr) {
                return;
            }
            vmaUnmapMemory(_allocator, _allocation);
            _pointer = nullptr;
        };

        T& operator[](size_t index) override
        {
            return _pointer[index];
        }

        T operator[](size_t index) const override
        {
            return _pointer[index];
        }

        T* raw() override
        {
            return _pointer;
        }

        void dispose() override
        {
            if (_pointer == nullptr) {
                return;
            }
            vmaUnmapMemory(_allocator, _allocation);
            _pointer = nullptr;
        }

        bool isValid() override
        {
            return _pointer != nullptr;
        }

        SimpleBufferMap& operator=(SimpleBufferMap&& other) noexcept
        {
            _allocator = other._allocator;
            _allocation = other._allocation;
            _pointer = other._pointer;
            other._pointer = nullptr;
            return *this;
        }
    };

    class SimpleBuffer : public Buffer, public VKResource
    {
        size_t _size;

        VkBuffer _buffer;

        VmaAllocator _allocator;
        VmaAllocation _allocation;

        bool _modifiable;

        std::optional<std::shared_ptr<BufferMap<char>>> rawMap(const CommandBuffer* commandBuffer = nullptr) override;

        std::optional<std::shared_ptr<BufferMap<char>>> rawMap(Range<uint32_t> range,
                                                               const CommandBuffer* commandBuffer = nullptr) override;

      public:
        SimpleBuffer(const SimpleBuffer& other) = delete;

        template<class T>
        SimpleBuffer(AbstractVKApplication* application, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                     const std::vector<T>& data) :
            SimpleBuffer(application, usage, properties, data.data(), static_cast<uint32_t>(data.size() * sizeof(T)))
        {
        }

        SimpleBuffer(AbstractVKApplication* application, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                     uint32_t sizeInBytes);

        SimpleBuffer(AbstractVKApplication* application, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                     const void* data, uint32_t sizeInBytes);

        ~SimpleBuffer() override;

        [[nodiscard]] size_t size() const override;

        [[nodiscard]] bool canBeWrittenOn() const override;

        [[nodiscard]] VkBuffer getRaw(std::shared_ptr<CommandBufferRun> run);
    };
} // namespace neon::vulkan

#endif // VULKANTEST_SIMPLEBUFFER_H
