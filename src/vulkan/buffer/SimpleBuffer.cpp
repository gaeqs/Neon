//
// Created by gaelr on 14/10/2022.
//

#include "SimpleBuffer.h"

#include <vulkan/util/VKUtil.h>
#include <cstring>

std::optional<std::shared_ptr<BufferMap<char>>> SimpleBuffer::rawMap() {
    return std::make_shared<SimpleBufferMap<char>>(_device, _vertexBufferMemory,
                                                   _size);
}

SimpleBuffer::SimpleBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                           VkBufferUsageFlags usage,
                           VkMemoryPropertyFlags properties,
                           uint32_t sizeInBytes)
        : SimpleBuffer(physicalDevice, device, usage, properties, nullptr,
                       sizeInBytes) {
}

SimpleBuffer::SimpleBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                           VkBufferUsageFlags usage,
                           VkMemoryPropertyFlags properties, const void* data,
                           uint32_t sizeInBytes) :
        Buffer(),
        _size(sizeInBytes),
        _physicalDevice(physicalDevice),
        _device(device),
        _vertexBuffer(VK_NULL_HANDLE),
        _vertexBufferMemory(VK_NULL_HANDLE),
        _modifiable(properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = _size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &_vertexBuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(_device, _vertexBuffer, &requirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = requirements.size;
    allocInfo.memoryTypeIndex = vulkan_util::findMemoryType(
            _physicalDevice,
            requirements.memoryTypeBits,
            properties
    );

    if (vkAllocateMemory(device, &allocInfo, nullptr,
                         &_vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error(
                "failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(device, _vertexBuffer, _vertexBufferMemory, 0);
    if (data != nullptr && sizeInBytes != 0 && _modifiable) {
        std::optional<std::shared_ptr<BufferMap<char>>> ptr = map<char>();
        memcpy(ptr.value()->raw(), data, _size);
    }

}

SimpleBuffer::~SimpleBuffer() {
    vkDestroyBuffer(_device, _vertexBuffer, nullptr);
    vkFreeMemory(_device, _vertexBufferMemory, nullptr);
}

size_t SimpleBuffer::size() const {
    return _size;
}

bool SimpleBuffer::canBeWrittenOn() const {
    return _modifiable;
}

VkBuffer SimpleBuffer::getRaw() const {
    return _vertexBuffer;
}

VkPhysicalDevice SimpleBuffer::getPhysicalDevice() const {
    return _physicalDevice;
}

VkDevice SimpleBuffer::getDevice() const {
    return _device;
}
