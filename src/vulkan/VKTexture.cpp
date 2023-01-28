//
// Created by gaelr on 10/11/2022.
//

#include "VKTexture.h"

#include <engine/Application.h>
#include <engine/structure/Room.h>

#include <vulkan/util/VKUtil.h>

uint32_t VKTexture::getPixelSize(TextureFormat format) {
    switch (format) {
        case TextureFormat::R32FG32FB32FA32F:
            return 16;
        case TextureFormat::R8G8B8:
        case TextureFormat::B8G8R8:
            return 3;
        case TextureFormat::R16FG16FB16F:
            return 6;
        case TextureFormat::R16FG16FB16FA16F:
            return 8;
        case TextureFormat::A8R8G8B8:
        case TextureFormat::B8G8R8A8:
        case TextureFormat::A8B8G8R8:
        case TextureFormat::R8G8B8A8:
        case TextureFormat::R8G8B8A8_SRGB:
        case TextureFormat::R32F:
        case TextureFormat::R16FG16F:
        case TextureFormat::DEPTH24STENCIL8:
        default:
            return 4;
    }
}

VKTexture::VKTexture(Application* application, const void* data,
                     int32_t width, int32_t height, TextureFormat format) :
        _vkApplication(&application->getImplementation()),
        _width(width),
        _height(height),
        _stagingBuffer(std::make_unique<SimpleBuffer>(
                _vkApplication,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                data,
                getPixelSize(format) * width * height
        )),
        _image(VK_NULL_HANDLE),
        _imageMemory(VK_NULL_HANDLE),
        _imageView(VK_NULL_HANDLE),
        _sampler(VK_NULL_HANDLE),
        _layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
        _external(false),
        _externalDirtyFlag(1) {

    VkFormat vkFormat = vulkan_util::getImageFormat(format);

    auto pair = vulkan_util::createImage(
            _vkApplication->getDevice(),
            _vkApplication->getPhysicalDevice(),
            width,
            height,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            vkFormat);

    _image = pair.first;
    _imageMemory = pair.second;

    vulkan_util::transitionImageLayout(
            _vkApplication,
            _image,
            vkFormat,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    vulkan_util::copyBufferToImage(
            _vkApplication,
            _stagingBuffer->getRaw(),
            _image,
            width,
            height
    );

    vulkan_util::transitionImageLayout(
            _vkApplication,
            _image,
            vkFormat,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );

    _imageView = vulkan_util::createImageView(
            _vkApplication->getDevice(),
            _image,
            vkFormat,
            VK_IMAGE_ASPECT_COLOR_BIT
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

VKTexture::VKTexture(Application* application,
                     int32_t width, int32_t height,
                     VkImageView imageView,
                     VkImageLayout layout) :
        _vkApplication(&application->getImplementation()),
        _width(width),
        _height(height),
        _stagingBuffer(nullptr),
        _image(VK_NULL_HANDLE),
        _imageMemory(VK_NULL_HANDLE),
        _imageView(imageView),
        _sampler(VK_NULL_HANDLE),
        _layout(layout),
        _external(true),
        _externalDirtyFlag(1) {

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(
            _vkApplication->getPhysicalDevice(),
            &properties
    );

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
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
    if (!_external) {
        vkDestroyImageView(_vkApplication->getDevice(), _imageView, nullptr);
        vkDestroyImage(_vkApplication->getDevice(), _image, nullptr);
        vkFreeMemory(_vkApplication->getDevice(), _imageMemory, nullptr);
    }
}

int32_t VKTexture::getWidth() const {
    return _width;
}

int32_t VKTexture::getHeight() const {
    return _height;
}

VkImageView VKTexture::getImageView() const {
    return _imageView;
}

VkSampler VKTexture::getSampler() const {
    return _sampler;
}

VkImageLayout VKTexture::getLayout() const {
    return _layout;
}

uint32_t VKTexture::getExternalDirtyFlag() const {
    return _externalDirtyFlag;
}

void VKTexture::changeExternalImageView(
        int32_t width, int32_t height, VkImageView imageView) {

    if (!_external) {
        std::cerr << "The image view of an internal texture cannot"
                     "be changed!" << std::endl;
        return;
    }

    _width = width;
    _height = height;
    _imageView = imageView;
    _externalDirtyFlag++;
    if (_externalDirtyFlag == 0) {
        _externalDirtyFlag++;
    }
}

void VKTexture::updateData(const void* data, int32_t width, int32_t height,
                           TextureFormat format) {

    if (_external) {
        std::cerr << "Couldn't update data of a texture"
                     " with an external handler" << std::endl;
        return;
    }

    auto map = _stagingBuffer->map<char>();
    if (map.has_value()) {
        memcpy(map.value()->raw(), data, getPixelSize(format) * width * height);
        map.value()->dispose();
    }

    auto vkFormat = vulkan_util::getImageFormat(format);
    vulkan_util::transitionImageLayout(
            _vkApplication,
            _image,
            vkFormat,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    vulkan_util::copyBufferToImage(
            _vkApplication,
            _stagingBuffer->getRaw(),
            _image,
            width,
            height
    );

    vulkan_util::transitionImageLayout(
            _vkApplication,
            _image,
            vkFormat,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
}
