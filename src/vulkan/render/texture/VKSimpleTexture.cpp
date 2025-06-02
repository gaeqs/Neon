//
// Created by gaeqs on 28/05/2025.
//

#include "VKSimpleTexture.h"
#include "VKTextureView.h"

#include <vulkan/render/buffer/SimpleBuffer.h>
#include <vulkan/util/VKUtil.h>
#include <vulkan/util/VulkanConversions.h>

namespace neon::vulkan
{
    namespace vc = conversions;

    void VKSimpleTexture::transitionLayout(VkImageLayout layout, VkCommandBuffer commandBuffer)
    {
        vulkan_util::transitionImageLayout(_image, vc::vkFormat(_info.format), _currentLayout, layout, _info.mipmaps,
                                           _info.layers, commandBuffer);
        _currentLayout = layout;
    }

    void VKSimpleTexture::uploadData(const std::byte* data, CommandBuffer* commandBuffer)
    {
        size_t size = _info.width * _info.height * _info.depth * _info.layers * vc::pixelSize(_info.format);
        SimpleBuffer buffer(getApplication(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 0, size);
        {
            auto map = buffer.map<std::byte>(commandBuffer);
            if (!map.has_value()) {
                return;
            }
            memcpy(map.value()->raw(), data, size);
        }

        VkCommandBuffer rawBuffer = commandBuffer->getImplementation().getCommandBuffer();

        transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, rawBuffer);

        vulkan_util::copyBufferToImage(buffer.getRaw(commandBuffer->getCurrentRun()), _image, {0, 0, 0},
                                       {_info.width, _info.height, _info.depth}, 0, _info.layers, rawBuffer);

        generateMipmaps(rawBuffer);
        // generateMipmaps transforms image to
        // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    }

    void VKSimpleTexture::generateMipmaps(VkCommandBuffer buffer)
    {
        vulkan_util::generateMipmaps(getApplication(), _image, _info.width, _info.height, _info.depth, _info.mipmaps,
                                     _info.layers, buffer);
        _currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    VKSimpleTexture::VKSimpleTexture(Application* application, std::string name, const ImageCreateInfo& info,
                                     const std::byte* data, CommandBuffer* commandBuffer) :
        VKResource(application),
        Texture(std::move(name)),
        _info(info),
        _currentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
    {
        VkImageUsageFlags usages = 0;
        for (const auto& usage : info.usages) {
            usages |= static_cast<VkImageUsageFlagBits>(usage);
        }

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = vc::vkImageType(info.dimensions);
        imageInfo.extent.width = info.width;
        imageInfo.extent.height = info.height;
        imageInfo.extent.depth = info.depth;
        imageInfo.mipLevels = info.mipmaps;
        imageInfo.arrayLayers = info.layers;
        imageInfo.format = vc::vkFormat(info.format);
        imageInfo.tiling = vc::vkImageTiling(info.tiling);
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usages;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = vc::vkSampleCountFlagBits(info.samples);

        if (info.viewType == TextureViewType::CUBE) {
            imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        } else if (info.viewType == TextureViewType::ARRAY_2D) {
            imageInfo.flags = VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
        }

        if (info.mipmaps == 0) {
            _info.mipmaps = std::bit_width(std::max({info.width, info.height, info.depth})) - 1;
            imageInfo.mipLevels = _info.mipmaps;
        }

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        auto alloc = getApplication()->getDevice()->getAllocator();
        vmaCreateImage(alloc, &imageInfo, &allocInfo, &_image, &_allocation, nullptr);

        CommandPoolHolder holder;
        if (commandBuffer == nullptr) {
            holder = getApplication()->getApplication()->getCommandManager().fetchCommandPool();
            commandBuffer = holder.getPool().beginCommandBuffer(true);
        }

        if (data != nullptr) {
            uploadData(data, commandBuffer);
        } else {
            VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            if (vulkan_util::isDepthFormat(info.format)) {
                layout = vulkan_util::isStencilFormat(info.format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                                                                   : VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
            }
            transitionLayout(layout, commandBuffer->getImplementation().getCommandBuffer());
        }
        if (holder.isValid()) {
            commandBuffer->end();
            commandBuffer->submit();
        }
    }

    VKSimpleTexture::~VKSimpleTexture()
    {
        auto device = rawDevice();
        auto bin = getApplication()->getBin();
        auto allocator = getApplication()->getDevice()->getAllocator();

        bin->destroyLater(device, getRuns(), [allocator, image = _image, allocation = _allocation] {
            vmaDestroyImage(allocator, image, allocation);
        });
    }

    rush::Vec3ui VKSimpleTexture::getDimensions() const
    {
        return {_info.width, _info.height, _info.depth};
    }

    TextureFormat VKSimpleTexture::getFormat() const
    {
        return _info.format;
    }

    SamplesPerTexel VKSimpleTexture::getSamplesPerTexel() const
    {
        return _info.samples;
    }

    size_t VKSimpleTexture::getNumberOfMipmaps() const
    {
        return _info.mipmaps;
    }

    std::optional<const TextureCapabilityRead*> VKSimpleTexture::asReadable() const
    {
        return {};
    }

    std::optional<TextureCapabilityModifiable*> VKSimpleTexture::asModifiable()
    {
        return this;
    }

    void* VKSimpleTexture::getNativeHandle()
    {
        return _image;
    }

    const void* VKSimpleTexture::getNativeHandle() const
    {
        return _image;
    }

    std::any VKSimpleTexture::getLayoutNativeHandle() const
    {
        return _currentLayout;
    }

    Result<void, std::string> VKSimpleTexture::updateData(const std::byte* data, rush::Vec3ui offset, rush::Vec3ui size,
                                                          uint32_t layerOffset, uint32_t layers,
                                                          CommandBuffer* commandBuffer)
    {
        if (offset.x() + size.x() > _info.width || offset.y() + size.y() > _info.height ||
            offset.z() + size.z() > _info.depth || layerOffset + layers > _info.layers) {
            return {"Texture update region is out of bounds."};
        }

        CommandPoolHolder holder;
        if (commandBuffer == nullptr) {
            holder = getApplication()->getApplication()->getCommandManager().fetchCommandPool();
            commandBuffer = holder.getPool().beginCommandBuffer(true);
        }

        size_t bytesSize = size.x() * size.y() * size.z() * layers * vc::pixelSize(_info.format);
        SimpleBuffer buffer(getApplication(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 0, bytesSize);
        {
            auto map = buffer.map<std::byte>(commandBuffer);
            if (!map.has_value()) {
                return {"Error mapping buffer."};
            }
            memcpy(map.value()->raw(), data, bytesSize);
        }

        VkCommandBuffer rawBuffer = commandBuffer->getImplementation().getCommandBuffer();
        transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, rawBuffer);
        vulkan_util::copyBufferToImage(buffer.getRaw(commandBuffer->getCurrentRun()), _image, offset.cast<int32_t>(),
                                       size, layerOffset, layers, rawBuffer);
        generateMipmaps(rawBuffer);

        if (holder.isValid()) {
            commandBuffer->end();
            commandBuffer->submit();
        }

        return {};
    }

    VkImage VKSimpleTexture::vk() const
    {
        return _image;
    }

    VkImageLayout VKSimpleTexture::vkLayout() const
    {
        return _currentLayout;
    }
} // namespace neon::vulkan