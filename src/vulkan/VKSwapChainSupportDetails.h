//
// Created by gaelr on 10/11/2022.
//

#ifndef NEON_VKSWAPCHAINSUPPORTDETAILS_H
#define NEON_VKSWAPCHAINSUPPORTDETAILS_H

#include <vector>
#include <vulkan/vulkan.h>

struct VKSwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

#endif //NEON_VKSWAPCHAINSUPPORTDETAILS_H
