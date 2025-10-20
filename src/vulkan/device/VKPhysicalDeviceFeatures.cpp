//
// Created by gaeqs on 20/09/2024.
//

#include "VKPhysicalDeviceFeatures.h"

#include <cstring>
#include <neon/logging/Logger.h>

namespace neon::vulkan
{
    VKFeatureHolder::VKFeatureHolder(const char* data, size_t size) :
        size(size),
        raw(new char[size])
    {
        memcpy(raw, data, size);
    }

    VKFeatureHolder::VKFeatureHolder(const VKFeatureHolder& other) :
        size(other.size),
        raw(new char[size])
    {
        memcpy(raw, other.raw, size);
    }

    VKFeatureHolder::~VKFeatureHolder()
    {
        delete[] raw;
    }

    VKFeatureHolder& VKFeatureHolder::operator=(const VKFeatureHolder& other)
    {
        delete[] raw;
        size = other.size;
        raw = new char[size];
        memcpy(raw, other.raw, size);
        return *this;
    }

    VKDefaultExtension* VKFeatureHolder::asDefaultExtensions()
    {
        return reinterpret_cast<VKDefaultExtension*>(raw);
    }

    const VKDefaultExtension* VKFeatureHolder::asDefaultExtensions() const
    {
        return reinterpret_cast<const VKDefaultExtension*>(raw);
    }

    VKPhysicalDeviceFeatures::VKPhysicalDeviceFeatures(const VKPhysicalDeviceFeatures& other) :
        basicFeatures(other.basicFeatures),
        features(other.features),
        extensions(other.extensions)
    {
        reweave();
    }

    VKPhysicalDeviceFeatures::VKPhysicalDeviceFeatures()
    {
        features.emplace_back(VkPhysicalDeviceVulkan11Features(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES));
        features.emplace_back(VkPhysicalDeviceVulkan12Features(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES));
        features.emplace_back(VkPhysicalDeviceVulkan13Features(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES));
        features.emplace_back(VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT));

        reweave();
    }

    VKPhysicalDeviceFeatures::VKPhysicalDeviceFeatures(VkPhysicalDevice device,
                                                       const std::vector<VKFeatureHolder>& extraFeatures)
    {
        VkPhysicalDeviceFeatures2 f{};
        f.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

        features.emplace_back(VkPhysicalDeviceVulkan11Features(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES));
        features.emplace_back(VkPhysicalDeviceVulkan12Features(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES));
        features.emplace_back(VkPhysicalDeviceVulkan13Features(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES));
        features.emplace_back(VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT));

        features.insert(features.end(), extraFeatures.begin(), extraFeatures.end());

        reweave();
        f.pNext = features.empty() ? nullptr : features[0].raw;

        vkGetPhysicalDeviceFeatures2(device, &f);

        basicFeatures = f.features;

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> rawExtensions(extensionCount);

        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, rawExtensions.data());

        extensions.reserve(extensionCount);
        for (auto& [name, _] : rawExtensions) {
            neon::debug() << "Physical device supported extensions: ";
            neon::debug() << " - " << name;
            extensions.emplace_back(name);
        }
    }

    bool VKPhysicalDeviceFeatures::hasExtension(const std::string& extension) const
    {
        const auto it = std::find(extensions.cbegin(), extensions.cend(), extension);
        return it != extensions.cend();
    }

    VkPhysicalDeviceFeatures2 VKPhysicalDeviceFeatures::toFeatures2() const
    {
        VkPhysicalDeviceFeatures2 f;
        f.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        f.features = basicFeatures;

        f.pNext = features.empty() ? nullptr : features[0].raw;
        return f;
    }

    VKPhysicalDeviceFeatures& VKPhysicalDeviceFeatures::operator=(const VKPhysicalDeviceFeatures& o)
    {
        basicFeatures = o.basicFeatures;
        features = o.features;
        extensions = o.extensions;
        reweave();
        return *this;
    }

    void VKPhysicalDeviceFeatures::reweave()
    {
        if (!features.empty()) {
            for (size_t i = 0; i < features.size() - 1; ++i) {
                features[i].asDefaultExtensions()->pNext = features[i + 1].raw;
            }
            features[features.size() - 1].asDefaultExtensions()->pNext = nullptr;
        }
    }
} // namespace neon::vulkan
