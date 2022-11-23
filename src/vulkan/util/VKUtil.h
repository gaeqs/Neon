//
// Created by gaelr on 11/11/2022.
//

#ifndef NEON_VKUTIL_H
#define NEON_VKUTIL_H


#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

struct InputDescription;

namespace vulkan_util {

    uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter,
                            VkMemoryPropertyFlags flags);

    VkCommandBuffer beginSingleTimeCommandBuffer(
            VkDevice device, VkCommandPool pool);

    void endSingleTimeCommandBuffer(
            VkDevice device, VkQueue queue,
            VkCommandPool pool, VkCommandBuffer buffer);

    void copyBuffer(VkDevice device, VkCommandPool pool, VkQueue queue,
                    VkBuffer source, VkBuffer destiny, VkDeviceSize size);

    void copyBuffer(VkDevice device, VkCommandPool pool, VkQueue queue,
                    VkBuffer source, VkBuffer destiny,
                    VkDeviceSize sourceOffset, VkDeviceSize destinyOffset,
                    VkDeviceSize size);

    void transitionImageLayout(
            VkDevice device, VkCommandPool pool, VkQueue queue,
            VkImage image, VkFormat format,
            VkImageLayout oldLayout, VkImageLayout newLayout);

    void copyBufferToImage(
            VkDevice device, VkCommandPool pool, VkQueue queue,
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    VkImageView createImageView(
            VkDevice device, VkImage image, VkFormat format);

    std::pair<VkVertexInputBindingDescription,
            std::vector<VkVertexInputAttributeDescription>>
    toVulkanDescription(uint32_t binding, uint32_t startLocation,
                        const InputDescription& description);

};


#endif //NEON_VKUTIL_H
