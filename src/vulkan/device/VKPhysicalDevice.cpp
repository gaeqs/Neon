//
// Created by gaeqs on 23/09/2024.
//

#include "VKPhysicalDevice.h"

namespace neon::vulkan {
    VKPhysicalDevice::VKPhysicalDevice()
        : _raw(VK_NULL_HANDLE), _properties() {}

    VKPhysicalDevice::VKPhysicalDevice(
        VkPhysicalDevice raw,
        VkSurfaceKHR surface,
        const std::vector<VKFeatureHolder>& extraFeatures)
        : _raw(raw),
          _features(raw, extraFeatures),
          _familyCollection(raw, surface) {
        vkGetPhysicalDeviceProperties(raw, &_properties);
    }

    VkPhysicalDevice VKPhysicalDevice::getRaw() const {
        return _raw;
    }

    const VkPhysicalDeviceProperties& VKPhysicalDevice::getProperties() const {
        return _properties;
    }

    const VKPhysicalDeviceFeatures& VKPhysicalDevice::getFeatures() const {
        return _features;
    }

    const VKQueueFamilyCollection&
    VKPhysicalDevice::getFamilyCollection() const {
        return _familyCollection;
    }
}
