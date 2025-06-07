//
// Created by gaelr on 14/10/2022.
//

#include "SimpleBuffer.h"

#include <cstring>

#include <vulkan/util/VKUtil.h>
#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan
{
    std::optional<std::shared_ptr<BufferMap<char>>> SimpleBuffer::rawMap(const CommandBuffer* commandBuffer)
    {
        return std::make_shared<SimpleBufferMap<char>>(_allocator, _allocation,
                                                       Range<uint32_t>(0, static_cast<uint32_t>(_size)));
    }

    std::optional<std::shared_ptr<BufferMap<char>>> SimpleBuffer::rawMap(Range<uint32_t> range,
                                                                         const CommandBuffer* commandBuffer)
    {
        return std::make_shared<SimpleBufferMap<char>>(_allocator, _allocation, range);
    }

    SimpleBuffer::SimpleBuffer(AbstractVKApplication* application, VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties, uint32_t sizeInBytes) :
        SimpleBuffer(application, usage, properties, nullptr, sizeInBytes)
    {
    }

    SimpleBuffer::SimpleBuffer(AbstractVKApplication* application, VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties, const void* data, uint32_t sizeInBytes) :
        VKResource(application),
        Buffer(),
        _size(sizeInBytes),
        _buffer(VK_NULL_HANDLE),
        _allocator(application->getDevice()->getAllocator()),
        _allocation(VK_NULL_HANDLE),
        _modifiable(properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = _size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        allocInfo.preferredFlags = properties;

        vmaCreateBuffer(_allocator, &bufferInfo, &allocInfo, &_buffer, &_allocation, nullptr);

        if (data != nullptr && sizeInBytes != 0 && _modifiable) {
            memcpy(map<char>().value()->raw(), data, _size);
        }
    }

    SimpleBuffer::~SimpleBuffer()
    {
        auto device = getApplication()->getDevice();
        auto bin = getApplication()->getBin();

        bin->destroyLater(device->getRaw(), getRuns(),
                          [allocator = _allocator, buffer = _buffer, allocation = _allocation] {
                              vmaDestroyBuffer(allocator, buffer, allocation);
                          });
    }

    size_t SimpleBuffer::size() const
    {
        return _size;
    }

    bool SimpleBuffer::canBeWrittenOn() const
    {
        return _modifiable;
    }

    VkBuffer SimpleBuffer::getRaw(std::shared_ptr<CommandBufferRun> run)
    {
        registerRun(std::move(run));
        return _buffer;
    }
} // namespace neon::vulkan