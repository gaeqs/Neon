//
// Created by gaelr on 11/11/2022.
//

#ifndef NEON_VKUTIL_H
#define NEON_VKUTIL_H

#include <vector>
#include <optional>
#include <memory>
#include <vulkan/vulkan.h>

#include <neon/render/texture/TextureCreateInfo.h>
#include <rush/vector/vec.h>
#include <vulkan/render/VKCommandBuffer.h>

namespace neon
{
    struct InputDescription;

    class FrameBuffer;
} // namespace neon

namespace neon::vulkan
{
    class AbstractVKApplication;
}

namespace neon::vulkan::vulkan_util
{
    uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags flags);

    void copyBuffer(VkCommandBuffer commandBuffer, VkBuffer source, VkBuffer destiny, VkDeviceSize size);

    void copyBuffer(VkCommandBuffer commandBuffer, VkBuffer source, VkBuffer destiny, VkDeviceSize sourceOffset,
                    VkDeviceSize destinyOffset, VkDeviceSize size);

    std::pair<VkImage, VkDeviceMemory> createImage(VkDevice device, VkPhysicalDevice physicalDevice,
                                                   const TextureCreateInfo& info, TextureViewType viewType,
                                                   VkFormat override = VK_FORMAT_UNDEFINED);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
                               uint32_t mipLevels, uint32_t layers, VkCommandBuffer commandBuffer);

    void copyBufferToImage(VkBuffer buffer, VkImage image, rush::Vec3i offset, rush::Vec3ui size, uint32_t baseLayer,
                           uint32_t layers, VkCommandBuffer commandBuffer);

    void copyImageToBuffer(VkBuffer buffer, VkImage image, rush::Vec3i offset, rush::Vec3ui size,
                           uint32_t layerOffset, uint32_t layers, VkCommandBuffer commandBuffer);

    void generateMipmaps(AbstractVKApplication* application, VkImage image, uint32_t width, uint32_t height,
                         uint32_t depth, uint32_t levels, uint32_t layers, VkCommandBuffer commandBuffer);

    std::optional<size_t> findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates,
                                              VkImageTiling tiling, VkFormatFeatureFlags features);

    bool isDepthFormat(TextureFormat format);

    bool isStencilFormat(TextureFormat format);

    VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                const TextureViewCreateInfo& info, TextureViewType type);

    std::pair<VkVertexInputBindingDescription, std::vector<VkVertexInputAttributeDescription>> toVulkanDescription(
        uint32_t binding, uint32_t startLocation, const InputDescription& description);

    void beginRenderPass(const VKCommandBuffer* commandBuffer, const std::shared_ptr<FrameBuffer>& fb, bool clear);
} // namespace neon::vulkan::vulkan_util

#endif // NEON_VKUTIL_H
