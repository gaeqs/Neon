//
// Created by gaelr on 10/11/2022.
//

#include "VKTexture.h"

#include <engine/Application.h>
#include <engine/Room.h>

#include <vulkan/util/VKUtil.h>

uint32_t VKTexture::getPixelSize(TextureFormat format) {
    switch (format) {
        case TextureFormat::RGB:
        case TextureFormat::BGR:
            return 3;
        case TextureFormat::ARGB:
        case TextureFormat::BGRA:
        case TextureFormat::ABGR:
        case TextureFormat::RGBA:
        default:
            return 4;
    }
}

VkFormat VKTexture::getImageFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::RGB:
            return VK_FORMAT_R8G8B8_SRGB;
        case TextureFormat::BGR:
            return VK_FORMAT_B8G8R8_SRGB;
        case TextureFormat::ARGB:
            throw std::runtime_error("Vulkan doesn't support ARGB!");
        case TextureFormat::BGRA:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case TextureFormat::ABGR:
            return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
        case TextureFormat::RGBA:
        default:
            return VK_FORMAT_R8G8B8A8_SRGB;
    }
}

VKTexture::VKTexture(Application* application, const void* data,
                     int32_t width, int32_t height, TextureFormat format) :
        _vkApplication(&application->getImplementation()),
        _width(width),
        _height(height),
        _stagingBuffer(
                _vkApplication->getPhysicalDevice(),
                _vkApplication->getDevice(),
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                data,
                getPixelSize(format) * width * height
        ),
        _image(VK_NULL_HANDLE),
        _imageMemory(VK_NULL_HANDLE),
        _imageView(VK_NULL_HANDLE),
        _sampler(VK_NULL_HANDLE) {

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;

    imageInfo.format = getImageFormat(format);
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT
                      | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    if (vkCreateImage(_stagingBuffer.getDevice(), &imageInfo,
                      nullptr, &_image) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_stagingBuffer.getDevice(),
                                 _image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vulkan_util::findMemoryType(
            _stagingBuffer.getPhysicalDevice(),
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    if (vkAllocateMemory(_stagingBuffer.getDevice(), &allocInfo, nullptr,
                         &_imageMemory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(_stagingBuffer.getDevice(), _image, _imageMemory, 0);

    vulkan_util::transitionImageLayout(
            _vkApplication->getDevice(),
            _vkApplication->getCommandPool(),
            _vkApplication->getGraphicsQueue(),
            _image,
            imageInfo.format,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    vulkan_util::copyBufferToImage(
            _vkApplication->getDevice(),
            _vkApplication->getCommandPool(),
            _vkApplication->getGraphicsQueue(),
            _stagingBuffer.getRaw(),
            _image,
            width,
            height
    );

    vulkan_util::transitionImageLayout(
            _vkApplication->getDevice(),
            _vkApplication->getCommandPool(),
            _vkApplication->getGraphicsQueue(),
            _image,
            imageInfo.format,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );

    _imageView = vulkan_util::createImageView(
            _vkApplication->getDevice(),
            _image,
            imageInfo.format
    );

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(
            _vkApplication->getPhysicalDevice(),
            &properties
    );

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(_vkApplication->getDevice(), &samplerInfo, nullptr,
                        &_sampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler!");
    }
}

VKTexture::~VKTexture() {
    vkDestroySampler(_vkApplication->getDevice(), _sampler, nullptr);
    vkDestroyImageView(_vkApplication->getDevice(), _imageView, nullptr);
    vkDestroyImage(_vkApplication->getDevice(), _image, nullptr);
    vkFreeMemory(_vkApplication->getDevice(), _imageMemory, nullptr);
}

int32_t VKTexture::getWidth() const {
    return _width;
}

int32_t VKTexture::getHeight() const {
    return _height;
}

void VKTexture::updateData(const void* data, int32_t width, int32_t height,
                           TextureFormat format) {

    auto map = _stagingBuffer.map<char>();
    if (map.has_value()) {
        memcpy(map.value()->raw(), data, getPixelSize(format) * width * height);
        map.value()->dispose();
    }

    auto vkFormat = getImageFormat(format);
    vulkan_util::transitionImageLayout(
            _vkApplication->getDevice(),
            _vkApplication->getCommandPool(),
            _vkApplication->getGraphicsQueue(),
            _image,
            vkFormat,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    vulkan_util::copyBufferToImage(
            _vkApplication->getDevice(),
            _vkApplication->getCommandPool(),
            _vkApplication->getGraphicsQueue(),
            _stagingBuffer.getRaw(),
            _image,
            width,
            height
    );

    vulkan_util::transitionImageLayout(
            _vkApplication->getDevice(),
            _vkApplication->getCommandPool(),
            _vkApplication->getGraphicsQueue(),
            _image,
            vkFormat,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
}

VkImageView VKTexture::getImageView() const {
    return _imageView;
}

VkSampler VKTexture::getSampler() const {
    return _sampler;
}
