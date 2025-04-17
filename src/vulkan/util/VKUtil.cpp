//
// Created by gaelr on 11/11/2022.
//

#include "VKUtil.h"

#include <stdexcept>

#include <neon/render/model/InputDescription.h>
#include <neon/render/buffer/FrameBuffer.h>
#include <vulkan/AbstractVKApplication.h>
#include <vulkan/util/VulkanConversions.h>
#include <vulkan/render/VKRenderPass.h>

namespace vc = neon::vulkan::conversions;

namespace neon::vulkan::vulkan_util
{
    uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags flags)
    {
        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties(device, &properties);

        for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (properties.memoryTypes[i].propertyFlags & flags) == flags) {
                return i;
            }
        }
        throw std::runtime_error("Failed to find suitable memory type!");
    }

    VkCommandBuffer beginSingleTimeCommandBuffer(VkDevice device, VkCommandPool pool)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = pool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer buffer;
        vkAllocateCommandBuffers(device, &allocInfo, &buffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(buffer, &beginInfo);

        return buffer;
    }

    void endSingleTimeCommandBuffer(VkDevice device, VkQueue queue, VkCommandPool pool, VkCommandBuffer buffer)
    {
        vkEndCommandBuffer(buffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buffer;

        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);

        vkFreeCommandBuffers(device, pool, 1, &buffer);
    }

    void copyBuffer(VkCommandBuffer commandBuffer, VkBuffer source, VkBuffer destiny, VkDeviceSize size)
    {
        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, source, destiny, 1, &copyRegion);
    }

    void copyBuffer(VkCommandBuffer commandBuffer, VkBuffer source, VkBuffer destiny, VkDeviceSize sourceOffset,
                    VkDeviceSize destinyOffset, VkDeviceSize size)
    {
        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.srcOffset = sourceOffset;
        copyRegion.dstOffset = destinyOffset;
        vkCmdCopyBuffer(commandBuffer, source, destiny, 1, &copyRegion);

        VkBufferMemoryBarrier memoryBarrier = {.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                                               .pNext = nullptr,
                                               .srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT,
                                               .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                                               .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                               .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                               .buffer = destiny,
                                               .offset = destinyOffset,
                                               .size = size};

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 1, &memoryBarrier, 0, nullptr);
    }

    std::pair<VkImage, VkDeviceMemory> createImage(VkDevice device, VkPhysicalDevice physicalDevice,
                                                   const ImageCreateInfo& info, TextureViewType viewType,
                                                   VkFormat override)
    {
        VkImage image;
        VkDeviceMemory memory;

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = vc::vkImageType(info.dimensions);
        imageInfo.extent.width = info.width;
        imageInfo.extent.height = info.height;
        imageInfo.extent.depth = info.depth;
        imageInfo.mipLevels = info.mipmaps;
        imageInfo.arrayLayers = info.layers;

        if (override == VK_FORMAT_UNDEFINED) {
            imageInfo.format = vc::vkFormat(info.format);
        } else {
            imageInfo.format = override;
        }

        VkImageUsageFlags usages = 0;
        for (const auto& usage : info.usages) {
            usages |= static_cast<VkImageUsageFlagBits>(usage);
        }

        imageInfo.tiling = vc::vkImageTiling(info.tiling);
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usages;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = vc::vkSampleCountFlagBits(info.samples);

        if (viewType == TextureViewType::CUBE) {
            imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        } else if (viewType == TextureViewType::ARRAY_2D) {
            imageInfo.flags = VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
        }

        if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = vulkan_util::findMemoryType(physicalDevice, memRequirements.memoryTypeBits,
                                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, memory, 0);
        return {image, memory};
    }

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
                               uint32_t mipLevels, uint32_t layers, VkCommandBuffer commandBuffer)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels == 0 ? 1 : mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layers;

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
                   newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }

            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

            barrier.srcAccessMask = 0;
            barrier.dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t depth,
                           uint32_t layers, VkCommandBuffer commandBuffer)
    {
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layers;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            depth,
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }

    void copyImageToBuffer(VkBuffer buffer, VkImage image, rush::Vec3i offset, rush::Vec<3, uint32_t> size,
                           uint32_t layerOffset, uint32_t layers, VkCommandBuffer commandBuffer)
    {
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = layerOffset;
        region.imageSubresource.layerCount = layers;

        region.imageOffset = {offset[0], offset[1], offset[2]};
        region.imageExtent = {
            size[0],
            size[1],
            size[2],
        };

        vkCmdCopyImageToBuffer(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1, &region);
    }

    void generateMipmaps(AbstractVKApplication* application, VkImage image, uint32_t width, uint32_t height,
                         uint32_t depth, uint32_t levels, int32_t layers, VkCommandBuffer commandBuffer)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layers;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = static_cast<uint32_t>(width);
        int32_t mipHeight = static_cast<uint32_t>(height);
        int32_t mipDepth = static_cast<uint32_t>(depth);

        for (uint32_t i = 1; i < levels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                                 nullptr, 0, nullptr, 1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = layers;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1,
                                  mipDepth > 1 ? mipDepth / 2 : 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = layers;

            vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 0, 0, nullptr, 0, nullptr, 1, &barrier);

            if (mipWidth > 1) {
                mipWidth /= 2;
            }
            if (mipHeight > 1) {
                mipHeight /= 2;
            }
        }

        barrier.subresourceRange.baseMipLevel = levels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                             nullptr, 0, nullptr, 1, &barrier);
    }

    std::optional<VkFormat> findSupportedFormat(VkPhysicalDevice physicalDevice,
                                                const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                                VkFormatFeatureFlags features)
    {
        for (const auto& format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if ((tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) ||
                (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)) {
                return format;
            }
        }

        return {};
    }

    VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                const ImageViewCreateInfo& info)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = vc::vkImageViewType(info.viewType);
        viewInfo.format = format;
        viewInfo.components.r = vc::vkComponentSwizzle(info.rSwizzle);
        viewInfo.components.g = vc::vkComponentSwizzle(info.gSwizzle);
        viewInfo.components.b = vc::vkComponentSwizzle(info.bSwizzle);
        viewInfo.components.a = vc::vkComponentSwizzle(info.aSwizzle);
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = info.baseMipmapLevel;
        viewInfo.subresourceRange.levelCount =
            info.mipmapLevelCount == 0 ? VK_REMAINING_MIP_LEVELS : info.mipmapLevelCount;
        viewInfo.subresourceRange.baseArrayLayer = info.baseArrayLayerLevel;
        viewInfo.subresourceRange.layerCount =
            info.arrayLayerCount == 0 ? VK_REMAINING_ARRAY_LAYERS : info.arrayLayerCount;

        VkImageView imageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture image view!");
        }

        return imageView;
    }

    std::pair<VkVertexInputBindingDescription, std::vector<VkVertexInputAttributeDescription>> toVulkanDescription(
        uint32_t binding, uint32_t startLocation, const InputDescription& description)
    {
        const VkFormat FORMATS[] = {VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT,
                                    VK_FORMAT_R32G32B32A32_SFLOAT};

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = binding;
        bindingDescription.stride = description.stride;
        bindingDescription.inputRate =
            description.rate == InputRate::VERTEX ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;

        std::vector<VkVertexInputAttributeDescription> attributes;

        uint32_t location = startLocation;
        for (const auto& attribute : description.attributes) {
            uint32_t size = attribute.sizeInFloats;
            uint32_t offset = attribute.offsetInBytes;

            while (size > 4) {
                VkVertexInputAttributeDescription vkAttribute{location++, binding, VK_FORMAT_R32G32B32A32_SFLOAT,
                                                              offset};
                attributes.push_back(vkAttribute);

                size -= 4;
                offset += sizeof(float) * 4;
            }

            VkVertexInputAttributeDescription vkAttribute{location++, binding, FORMATS[size - 1], offset};
            attributes.push_back(vkAttribute);
        }

        return {bindingDescription, attributes};
    }

    void beginRenderPass(const VKCommandBuffer* commandBuffer, const std::shared_ptr<FrameBuffer>& fb, bool clear)
    {
        auto& frameBuffer = fb->getImplementation();
        auto& renderPass = frameBuffer.getRenderPass();

        frameBuffer.registerRun(commandBuffer->getCurrentRun());

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass.getRaw();
        renderPassInfo.framebuffer = frameBuffer.getRaw();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {frameBuffer.getWidth(), frameBuffer.getHeight()};

        std::vector<VkClearValue> clearValues;
        if (clear) {
            auto outputs = frameBuffer.getOutputs();
            clearValues.reserve(outputs.size() * 2);

            for (uint32_t i = 0; i < outputs.size(); ++i) {
                auto& output = outputs[i];
                VkClearValue value;
                if (output.type == FrameBufferOutputType::COLOR || output.type == FrameBufferOutputType::SWAP) {
                    auto clearColor = fb->getClearColor(i);
                    if (clearColor.has_value()) {
                        auto c = clearColor.value();
                        value.color = {c.x(), c.y(), c.z(), c.w()};
                    } else {
                        value.color = {0.0f, 0.0f, 0.0f, 1.0f};
                    }
                } else if (output.type == FrameBufferOutputType::DEPTH) {
                    auto [depth, stencil] = fb->getDepthClearColor();
                    value.depthStencil = {depth, stencil};
                }

                clearValues.push_back(value);
                if (output.texture != output.resolvedTexture) {
                    clearValues.push_back(value);
                }
            }

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();
        } else {
            renderPassInfo.clearValueCount = 0;
        }

        auto cmd = commandBuffer->getCommandBuffer();
        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(frameBuffer.getWidth());
        viewport.height = static_cast<float>(frameBuffer.getHeight());
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = {frameBuffer.getWidth(), frameBuffer.getHeight()};
        vkCmdSetScissor(cmd, 0, 1, &scissor);
    }
} // namespace neon::vulkan::vulkan_util
