//
// Created by gaelr on 11/11/2022.
//

#ifndef NEON_VKUTIL_H
#define NEON_VKUTIL_H


#include <cstdint>
#include <vector>
#include <optional>
#include <memory>
#include <vulkan/vulkan.h>

#include <engine/render/TextureCreateInfo.h>

namespace neon {
    struct InputDescription;

    class FrameBuffer;
}

namespace neon::vulkan {
    class AbstractVKApplication;
}

namespace neon::vulkan::vulkan_util {
    uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter,
                            VkMemoryPropertyFlags flags);

    VkCommandBuffer beginSingleTimeCommandBuffer(
            VkDevice device, VkCommandPool pool);

    void endSingleTimeCommandBuffer(
            VkDevice device, VkQueue queue,
            VkCommandPool pool, VkCommandBuffer buffer);

    void copyBuffer(VkCommandBuffer commandBuffer,
                    VkBuffer source, VkBuffer destiny, VkDeviceSize size);

    void copyBuffer(VkCommandBuffer commandBuffer,
                    VkBuffer source, VkBuffer destiny,
                    VkDeviceSize sourceOffset, VkDeviceSize destinyOffset,
                    VkDeviceSize size);

    std::pair<VkImage, VkDeviceMemory> createImage(
            VkDevice device,
            VkPhysicalDevice physicalDevice,
            const ImageCreateInfo& info,
            TextureViewType viewType,
            VkFormat override = VK_FORMAT_UNDEFINED);

    void transitionImageLayout(
            AbstractVKApplication* application,
            VkImage image, VkFormat format,
            VkImageLayout oldLayout, VkImageLayout newLayout,
            uint32_t mipLevels,
            uint32_t layers);

    void copyBufferToImage(
            AbstractVKApplication* application,
            VkBuffer buffer, VkImage image,
            uint32_t width, uint32_t height, uint32_t depth,
            uint32_t layers);

    void generateMipmaps(
            AbstractVKApplication* application,
            VkImage image,
            uint32_t width, uint32_t height, uint32_t depth,
            uint32_t levels, int32_t layers);

    std::optional<VkFormat> findSupportedFormat(
            VkPhysicalDevice physicalDevice,
            const std::vector<VkFormat>& candidates,
            VkImageTiling tiling,
            VkFormatFeatureFlags features);

    VkImageView createImageView(
            VkDevice device,
            VkImage image,
            VkFormat format,
            VkImageAspectFlags aspectFlags,
            const ImageViewCreateInfo& info);

    std::pair<VkVertexInputBindingDescription,
            std::vector<VkVertexInputAttributeDescription>>
    toVulkanDescription(uint32_t binding, uint32_t startLocation,
                        const InputDescription& description);

    void beginRenderPass(VkCommandBuffer commandBuffer,
                         const std::shared_ptr<FrameBuffer>& fb,
                         bool clear);
}


#endif //NEON_VKUTIL_H
