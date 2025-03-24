//
// Created by gaelr on 10/11/2022.
//

#include "VKSwapChainSupportDetails.h"

namespace neon::vulkan
{
    VKSwapChainSupportDetails::VKSwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        uint32_t formatCount;
        uint32_t presentModeCount;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data());
        }

        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes.data());
        }
    }
} // namespace neon::vulkan
