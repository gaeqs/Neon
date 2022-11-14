//
// Created by gaelr on 11/11/2022.
//

#ifndef NEON_VKUTIL_H
#define NEON_VKUTIL_H


#include <cstdint>
#include <vulkan/vulkan.h>

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

    void transitionImageLayout(
            VkDevice device, VkCommandPool pool, VkQueue queue,
            VkImage image, VkFormat format,
            VkImageLayout oldLayout, VkImageLayout newLayout);

    void copyBufferToImage(
            VkDevice device, VkCommandPool pool, VkQueue queue,
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    VkImageView createImageView(
            VkDevice device, VkImage image, VkFormat format);

};


#endif //NEON_VKUTIL_H
