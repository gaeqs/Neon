//
// Created by gaelr on 14/10/2022.
//

#include "SimpleBuffer.h"

#include <cstring>

#include <vulkan/util/VKUtil.h>
#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan {
    std::optional<std::shared_ptr<BufferMap<char>>> SimpleBuffer::rawMap(
            const CommandBuffer* commandBuffer) {
        return std::make_shared<SimpleBufferMap<char>>(
                _application->getDevice()->getRaw(), _vertexBufferMemory,
                Range<uint32_t>(0, _size));
    }

    std::optional<std::shared_ptr<BufferMap<char>>>
    SimpleBuffer::rawMap(Range<uint32_t> range,
                         const CommandBuffer* commandBuffer) {
        return std::make_shared<SimpleBufferMap<char>>(
                _application->getDevice()->getRaw(), _vertexBufferMemory, range);
    }

    SimpleBuffer::SimpleBuffer(AbstractVKApplication* application,
                               VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties,
                               uint32_t sizeInBytes)
            : SimpleBuffer(application, usage, properties, nullptr,
                           sizeInBytes) {
    }

    SimpleBuffer::SimpleBuffer(AbstractVKApplication* application,
                               VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties,
                               const void* data,
                               uint32_t sizeInBytes) :
            Buffer(),
            _size(sizeInBytes),
            _application(application),
            _vertexBuffer(VK_NULL_HANDLE),
            _vertexBufferMemory(VK_NULL_HANDLE),
            _modifiable(properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {

        VkDevice device = _application->getDevice()->getRaw();

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = _size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device,
                           &bufferInfo, nullptr, &_vertexBuffer) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(device,
                                      _vertexBuffer, &requirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = requirements.size;
        allocInfo.memoryTypeIndex = vulkan_util::findMemoryType(
                _application->getPhysicalDevice(),
                requirements.memoryTypeBits,
                properties
        );

        if (vkAllocateMemory(device, &allocInfo, nullptr,
                             &_vertexBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error(
                    "failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(device,
                           _vertexBuffer, _vertexBufferMemory, 0);
        if (data != nullptr && sizeInBytes != 0 && _modifiable) {
            memcpy(map<char>().value()->raw(), data, _size);
        }

    }

    SimpleBuffer::~SimpleBuffer() {
        VkDevice device = _application->getDevice()->getRaw();

        vkDestroyBuffer(device, _vertexBuffer, nullptr);
        vkFreeMemory(device, _vertexBufferMemory, nullptr);

        _vertexBuffer = VK_NULL_HANDLE;
        _vertexBufferMemory = VK_NULL_HANDLE;
    }

    size_t SimpleBuffer::size() const {
        return _size;
    }

    bool SimpleBuffer::canBeWrittenOn() const {
        return _modifiable;
    }

    AbstractVKApplication* SimpleBuffer::getApplication() const {
        return _application;
    }

    VkBuffer SimpleBuffer::getRaw() const {
        return _vertexBuffer;
    }
}

VkResult neon::vulkan::simple_buffer::mapMemory(
        VkDevice pT, VkDeviceMemory pT1, uint32_t from,
        uint32_t size, int i, void** pVoid) {
    return vkMapMemory(pT, pT1, from, size, i, pVoid);
}

void neon::vulkan::simple_buffer::unmapMemory(
        VkDevice device, VkDeviceMemory memory) {
    vkUnmapMemory(device, memory);
}
