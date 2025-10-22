//
// Created by gaeqs on 6/08/24.
//

#ifndef VKDEVICE_H
#define VKDEVICE_H

#include <vulkan/vulkan.h>

#include <vma/vk_mem_alloc.h>

#include <vulkan/queue/VKQueueFamilyCollection.h>
#include <vulkan/queue/VKQueueProvider.h>
#include <vulkan/device/VKPhysicalDeviceFeatures.h>

namespace neon::vulkan
{

    class VKDeviceHolder
    {
        std::unique_lock<std::mutex> _lock;
        VkDevice _device;

      public:
        VKDeviceHolder(const VKDeviceHolder& other) = delete;

        VKDeviceHolder(VKDeviceHolder&& other) noexcept;

        VKDeviceHolder& operator=(VKDeviceHolder&& other) noexcept;

        VKDeviceHolder(std::mutex& mutex, VkDevice device);

        VkDevice get() const;

        operator VkDevice() const;
    };

    class VKDevice
    {
        std::mutex _holdMutex;
        VkDevice _raw;
        std::unique_ptr<VKQueueProvider> _queueProvider;
        VKPhysicalDeviceFeatures _enabledFeatures;
        VmaAllocator _allocator;
        bool _external;

        void createAllocator(VkInstance instance, VkPhysicalDevice physicalDevice);

      public:
        VKDevice(const VKDevice& other) = delete;

        VKDevice(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice raw,
                 const VKQueueFamilyCollection& families, const VKPhysicalDeviceFeatures& features,
                 const std::vector<uint32_t>& presentQueues);

        VKDevice(VkInstance instance, VkPhysicalDevice physicalDevice, const VKPhysicalDeviceFeatures& features,
                 const VKQueueFamilyCollection& families);

        ~VKDevice();

        [[nodiscard]] VKDeviceHolder hold();

        [[nodiscard]] VkDevice getDeviceWithoutHolding() const;

        [[nodiscard]] VmaAllocator getAllocator() const;

        [[nodiscard]] VKQueueProvider* getQueueProvider() const;

        [[nodiscard]] const VKPhysicalDeviceFeatures& getEnabledFeatures() const;
    };
} // namespace neon::vulkan

#endif // VKDEVICE_H
