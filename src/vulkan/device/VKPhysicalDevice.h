//
// Created by gaeqs on 23/09/2024.
//

#ifndef VKPHYSICALDEVICE_H
#define VKPHYSICALDEVICE_H

#include <vulkan/vulkan.h>

#include <vulkan/device/VKPhysicalDeviceFeatures.h>
#include <vulkan/queue/VKQueueFamilyCollection.h>

namespace neon::vulkan
{
    class VKPhysicalDevice
    {
        VkPhysicalDevice _raw;
        VkPhysicalDeviceProperties _properties;
        VKPhysicalDeviceFeatures _features;
        VKQueueFamilyCollection _familyCollection;

      public:
        VKPhysicalDevice();

        VKPhysicalDevice(VkPhysicalDevice raw, VkSurfaceKHR surface,
                         const std::vector<VKFeatureHolder>& extraFeatures = {});

        [[nodiscard]] VkPhysicalDevice getRaw() const;

        [[nodiscard]] const VkPhysicalDeviceProperties& getProperties() const;

        [[nodiscard]] const VKPhysicalDeviceFeatures& getFeatures() const;

        [[nodiscard]] const VKQueueFamilyCollection& getFamilyCollection() const;
    };
} // namespace neon::vulkan

#endif //VKPHYSICALDEVICE_H
