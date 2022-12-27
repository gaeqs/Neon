//
// Created by gaelr on 11/11/2022.
//

#ifndef NEON_VKUTIL_H
#define NEON_VKUTIL_H


#include <cstdint>
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

#include <engine/TextureFormat.h>

class VKApplication;

struct InputDescription;

namespace vulkan_util {

    uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter,
                            VkMemoryPropertyFlags flags);

    VkCommandBuffer beginSingleTimeCommandBuffer(
            VkDevice device, VkCommandPool pool);

    void endSingleTimeCommandBuffer(
            VkDevice device, VkQueue queue,
            VkCommandPool pool, VkCommandBuffer buffer);

    void copyBuffer(VKApplication* application,
                    VkBuffer source, VkBuffer destiny, VkDeviceSize size);

    void copyBuffer(VKApplication* application,
                    VkBuffer source, VkBuffer destiny,
                    VkDeviceSize sourceOffset, VkDeviceSize destinyOffset,
                    VkDeviceSize size);

    std::pair<VkImage, VkDeviceMemory> createImage(
            VkDevice device,
            VkPhysicalDevice physicalDevice,
            uint32_t width,
            uint32_t height,
            VkImageUsageFlags usage,
            VkFormat format);

    void transitionImageLayout(
            VKApplication* application,
            VkImage image, VkFormat format,
            VkImageLayout oldLayout, VkImageLayout newLayout);

    void copyBufferToImage(
            VKApplication* application,
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    std::optional<VkFormat> findSupportedFormat(
            VkPhysicalDevice physicalDevice,
            const std::vector<VkFormat>& candidates,
            VkImageTiling tiling,
            VkFormatFeatureFlags features);

    VkImageView createImageView(
            VkDevice device,
            VkImage image,
            VkFormat format,
            VkImageAspectFlags aspectFlags);

    std::pair<VkVertexInputBindingDescription,
            std::vector<VkVertexInputAttributeDescription>>
    toVulkanDescription(uint32_t binding, uint32_t startLocation,
                        const InputDescription& description);

    VkFormat getImageFormat(TextureFormat format);

    std::vector<VkFormat> getImageFormats(
            const std::vector<TextureFormat>& formats);

}


#endif //NEON_VKUTIL_H
