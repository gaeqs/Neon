//
// Created by gaeqs on 20/09/2024.
//

#ifndef VKPHYSICALDEVICEFEATURES_H
#define VKPHYSICALDEVICEFEATURES_H

#include <vulkan/vulkan.h>

namespace neon::vulkan {
    class VKPhysicalDeviceFeatures {
        VkPhysicalDeviceFeatures2 _features = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2
        };
        VkPhysicalDeviceVulkan11Features _vulkan11Features = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES
        };
        VkPhysicalDeviceVulkan12Features _vulkan12Features = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
        };
        VkPhysicalDeviceVulkan13Features _vulkan13Features = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES
        };
        VkPhysicalDeviceMeshShaderFeaturesEXT _meshShaderFeature = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT
        };

    public:
        VKPhysicalDeviceFeatures(const VKPhysicalDeviceFeatures& other);

        VKPhysicalDeviceFeatures();

        explicit VKPhysicalDeviceFeatures(VkPhysicalDevice device);

        [[nodiscard]] const VkPhysicalDeviceFeatures2& getFeatures() const;

        [[nodiscard]] VkPhysicalDeviceFeatures2& getFeatures();

        [[nodiscard]] const VkPhysicalDeviceVulkan11Features&
        getVulkan11Features() const;

        [[nodiscard]] VkPhysicalDeviceVulkan11Features&
        getVulkan11Features();

        [[nodiscard]] const VkPhysicalDeviceVulkan12Features&
        getVulkan12Features() const;

        [[nodiscard]] VkPhysicalDeviceVulkan12Features&
        getVulkan12Features();

        [[nodiscard]] const VkPhysicalDeviceVulkan13Features&
        getVulkan13Features() const;

        [[nodiscard]] VkPhysicalDeviceVulkan13Features&
        getVulkan13Features();

        [[nodiscard]] const VkPhysicalDeviceMeshShaderFeaturesEXT&
        getMeshShaderFeature() const;

        [[nodiscard]] VkPhysicalDeviceMeshShaderFeaturesEXT&
        getMeshShaderFeature();

        VKPhysicalDeviceFeatures& operator=(const VKPhysicalDeviceFeatures& other);
    };
}


#endif //VKPHYSICALDEVICEFEATURES_H
