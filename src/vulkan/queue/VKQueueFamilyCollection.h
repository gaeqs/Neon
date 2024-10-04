//
// Created by gaeqs on 4/08/24.
//

#ifndef VKQUEUEFAMILYCOLLECTION_H
#define VKQUEUEFAMILYCOLLECTION_H

#include <vector>
#include <vulkan/vulkan.h>

#include <vulkan/queue/VKQueueFamily.h>

namespace neon::vulkan {
    class VKQueueFamilyCollection {
        std::vector<VKQueueFamily> _families;

    public:
        VKQueueFamilyCollection() = default;

        VKQueueFamilyCollection(VkPhysicalDevice device, VkSurfaceKHR surface);

        VKQueueFamilyCollection(
            const std::vector<VkQueueFamilyProperties>& families,
            VkPhysicalDevice device,
            VkSurfaceKHR surface);

        [[nodiscard]] const std::vector<VKQueueFamily>& getFamilies() const;

        [[nodiscard]] std::optional<const VKQueueFamily*>
        getCompatibleQueueFamily(
            const VKQueueFamily::Capabilities& capabilities) const;

        [[nodiscard]] std::vector<const VKQueueFamily*>
        getAllCompatibleQueueFamilies(
            const VKQueueFamily::Capabilities& capabilities) const;

        void getAllCompatibleQueueFamilies(
            const VKQueueFamily::Capabilities& capabilities,
            std::vector<const VKQueueFamily*>& vector) const;
    };
}


#endif //VKQUEUEFAMILYCOLLECTION_H
