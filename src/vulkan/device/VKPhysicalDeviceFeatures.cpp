//
// Created by gaeqs on 20/09/2024.
//

#include "VKPhysicalDeviceFeatures.h"

#include <neon/logging/Logger.h>

namespace neon::vulkan {
    VKPhysicalDeviceFeatures::VKPhysicalDeviceFeatures(
        const VKPhysicalDeviceFeatures& other)
        : _features(other._features),
          _vulkan11Features(other._vulkan11Features),
          _vulkan12Features(other._vulkan12Features),
          _vulkan13Features(other._vulkan13Features),
          _meshShaderFeature(other._meshShaderFeature) {
        _features.pNext = &_vulkan11Features;
        _vulkan11Features.pNext = &_vulkan12Features;
        _vulkan12Features.pNext = &_vulkan13Features;
        _vulkan13Features.pNext = &_meshShaderFeature;
        _meshShaderFeature.pNext = nullptr;
        Logger::defaultLogger()->debug("CALL");
    }

    VKPhysicalDeviceFeatures::VKPhysicalDeviceFeatures() {}

    VKPhysicalDeviceFeatures::VKPhysicalDeviceFeatures(
        VkPhysicalDevice device) {
        _features.pNext = &_vulkan11Features;
        _vulkan11Features.pNext = &_vulkan12Features;
        _vulkan12Features.pNext = &_vulkan13Features;
        _vulkan13Features.pNext = &_meshShaderFeature;
        _meshShaderFeature.pNext = nullptr;
        vkGetPhysicalDeviceFeatures2(device, &_features);
    }

    const VkPhysicalDeviceFeatures2&
    VKPhysicalDeviceFeatures::getFeatures() const {
        return _features;
    }

    VkPhysicalDeviceFeatures2& VKPhysicalDeviceFeatures::getFeatures() {
        return _features;
    }

    const VkPhysicalDeviceVulkan11Features&
    VKPhysicalDeviceFeatures::getVulkan11Features() const {
        return _vulkan11Features;
    }

    VkPhysicalDeviceVulkan11Features&
    VKPhysicalDeviceFeatures::getVulkan11Features() {
        return _vulkan11Features;
    }

    const VkPhysicalDeviceVulkan12Features&
    VKPhysicalDeviceFeatures::getVulkan12Features() const {
        return _vulkan12Features;
    }

    VkPhysicalDeviceVulkan12Features&
    VKPhysicalDeviceFeatures::getVulkan12Features() {
        return _vulkan12Features;
    }

    const VkPhysicalDeviceVulkan13Features&
    VKPhysicalDeviceFeatures::getVulkan13Features() const {
        return _vulkan13Features;
    }

    VkPhysicalDeviceVulkan13Features&
    VKPhysicalDeviceFeatures::getVulkan13Features() {
        return _vulkan13Features;
    }

    const VkPhysicalDeviceMeshShaderFeaturesEXT&
    VKPhysicalDeviceFeatures::getMeshShaderFeature() const {
        return _meshShaderFeature;
    }

    VkPhysicalDeviceMeshShaderFeaturesEXT&
    VKPhysicalDeviceFeatures::getMeshShaderFeature() {
        return _meshShaderFeature;
    }

    VKPhysicalDeviceFeatures& VKPhysicalDeviceFeatures::operator=(
        const VKPhysicalDeviceFeatures& other) {
        _features = other._features;
        _vulkan11Features = other._vulkan11Features;
        _vulkan12Features = other._vulkan12Features;
        _vulkan13Features = other._vulkan13Features;
        _meshShaderFeature = other._meshShaderFeature;

        _features.pNext = &_vulkan11Features;
        _vulkan11Features.pNext = &_vulkan12Features;
        _vulkan12Features.pNext = &_vulkan13Features;
        _vulkan13Features.pNext = &_meshShaderFeature;
        _meshShaderFeature.pNext = nullptr;

        return *this;
    }
}
