//
// Created by gaeqs on 6/08/24.
//

#ifndef VKDEVICE_H
#define VKDEVICE_H

#include <vulkan/vulkan.h>

#include <vulkan/queue/VKQueueFamilyCollection.h>
#include <vulkan/queue/VKQueueProvider.h>
#include <vulkan/device/VKPhysicalDeviceFeatures.h>

namespace neon::vulkan
{
    class VKDevice
    {
        VkDevice _raw;
        std::unique_ptr<VKQueueProvider> _queueProvider;
        VKPhysicalDeviceFeatures _enabledFeatures;
        bool _external;

      public:
        VKDevice(VkDevice raw, const VKQueueFamilyCollection& families, const VKPhysicalDeviceFeatures& features,
                 const std::vector<uint32_t>& presentQueues);

        VKDevice(VkPhysicalDevice physicalDevice, const VKPhysicalDeviceFeatures& features,
                 const VKQueueFamilyCollection& families);

        ~VKDevice();

        [[nodiscard]] VkDevice getRaw() const;

        [[nodiscard]] VKQueueProvider* getQueueProvider() const;

        [[nodiscard]] const VKPhysicalDeviceFeatures& getEnabledFeatures() const;
    };
} // namespace neon::vulkan

#endif //VKDEVICE_H
