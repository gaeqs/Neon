//
// Created by gaeqs on 4/08/24.
//

#include "VKQueueFamilyCollection.h"

#include <cstdint>
#include <neon/logging/Logger.h>

namespace neon::vulkan {
    VKQueueFamilyCollection::VKQueueFamilyCollection(VkPhysicalDevice device,
        VkSurfaceKHR surface) {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

        _families.reserve(count);

        std::vector<VkQueueFamilyProperties> raw(count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, raw.data());

        for (auto properties: raw) {
            _families.emplace_back(
                device,
                surface,
                _families.size(),
                properties
            );
        }
    }

    VKQueueFamilyCollection::VKQueueFamilyCollection(
        const std::vector<VkQueueFamilyProperties>& families,
        VkPhysicalDevice device,
        VkSurfaceKHR surface) {
        _families.reserve(families.size());

        for (auto properties: families) {
            _families.emplace_back(
                device,
                surface,
                _families.size(),
                properties
            );
        }
    }

    const std::vector<VKQueueFamily>& VKQueueFamilyCollection::
    getFamilies() const {
        return _families;
    }

    std::optional<const VKQueueFamily*> VKQueueFamilyCollection::
    getCompatibleQueueFamily(
        const VKQueueFamily::Capabilities& capabilities) const {
        for (auto& family: _families) {
            if (family.getCapabilities().isCompatible(capabilities)) {
                return {&family};
            }
        }
        return {};
    }

    std::vector<const VKQueueFamily*> VKQueueFamilyCollection::
    getAllCompatibleQueueFamilies(
        const VKQueueFamily::Capabilities& capabilities) const {
        std::vector<const VKQueueFamily*> list;

        for (auto family: _families) {
            if (family.getCapabilities().isCompatible(capabilities)) {
                list.push_back(&family);
            }
        }

        return list;
    }

    void VKQueueFamilyCollection::getAllCompatibleQueueFamilies(
        const VKQueueFamily::Capabilities& capabilities,
        std::vector<const VKQueueFamily*>& vector) const {
        for (auto& family: _families) {
            if (family.getCapabilities().isCompatible(capabilities)) {
                vector.push_back(&family);
            }
        }
    }
}
