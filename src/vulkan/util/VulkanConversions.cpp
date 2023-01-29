//
// Created by gaelr on 28/01/2023.
//

#include "VulkanConversions.h"

#include <stdexcept>

#include <engine/render/TextureCreateInfo.h>

namespace vc = vulkan_conversions;

uint32_t vc::pixelSize(const TextureFormat& format) {
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

VkFormat vc::vkFormat(const TextureFormat& format) {
    switch (format) {
        case TextureFormat::R8G8B8:
            return VK_FORMAT_R8G8B8_UNORM;
        case TextureFormat::B8G8R8:
            return VK_FORMAT_B8G8R8_UNORM;
        case TextureFormat::A8R8G8B8:
            throw std::runtime_error("Vulkan doesn't support ARGB!");
        case TextureFormat::B8G8R8A8:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case TextureFormat::A8B8G8R8:
            return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
        case TextureFormat::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case TextureFormat::R8G8B8A8:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case TextureFormat::R32FG32FB32FA32F:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case TextureFormat::R32F:
            return VK_FORMAT_R32_SFLOAT;
        case TextureFormat::R16FG16F:
            return VK_FORMAT_R16G16_SFLOAT;
        case TextureFormat::R16FG16FB16F:
            return VK_FORMAT_R16G16B16_SFLOAT;
        case TextureFormat::R16FG16FB16FA16F:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case TextureFormat::DEPTH24STENCIL8:
            return VK_FORMAT_D24_UNORM_S8_UINT;

        default:
            throw std::runtime_error("Conversion not found!");
    }
}

std::vector<VkFormat> vc::vkFormat(const std::vector<TextureFormat>& formats) {
    std::vector<VkFormat> map;
    map.reserve(formats.size());
    for (const auto& item: formats) {
        map.push_back(vkFormat(item));
    }
    return map;
}

VkImageTiling vc::vkImageTiling(const Tiling& tiling) {
    switch (tiling) {
        case Tiling::OPTIMAL:
            return VK_IMAGE_TILING_OPTIMAL;
        case Tiling::LINEAR:
            return VK_IMAGE_TILING_LINEAR;
        case Tiling::DRM:
            return VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkImageType vc::vkImageType(const TextureDimensions& dimensions) {
    switch (dimensions) {
        case TextureDimensions::ONE:
            return VK_IMAGE_TYPE_1D;
        case TextureDimensions::TWO:
            return VK_IMAGE_TYPE_2D;
        case TextureDimensions::THREE:
            return VK_IMAGE_TYPE_3D;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkSampleCountFlagBits
vc::vkSampleCountFlagBits(const SamplesPerTexel& samples) {
    switch (samples) {
        case SamplesPerTexel::COUNT_1:
            return VK_SAMPLE_COUNT_1_BIT;
        case SamplesPerTexel::COUNT_2:
            return VK_SAMPLE_COUNT_2_BIT;
        case SamplesPerTexel::COUNT_4:
            return VK_SAMPLE_COUNT_4_BIT;
        case SamplesPerTexel::COUNT_8:
            return VK_SAMPLE_COUNT_8_BIT;
        case SamplesPerTexel::COUNT_16:
            return VK_SAMPLE_COUNT_16_BIT;
        case SamplesPerTexel::COUNT_32:
            return VK_SAMPLE_COUNT_32_BIT;
        case SamplesPerTexel::COUNT_64:
            return VK_SAMPLE_COUNT_64_BIT;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkImageViewType vc::vkImageViewType(const TextureViewType& viewType) {
    switch (viewType) {
        case TextureViewType::NORMAL_1D:
            return VK_IMAGE_VIEW_TYPE_1D;
        case TextureViewType::NORMAL_2D:
            return VK_IMAGE_VIEW_TYPE_2D;
        case TextureViewType::NORMAL_3D:
            return VK_IMAGE_VIEW_TYPE_3D;
        case TextureViewType::CUBE:
            return VK_IMAGE_VIEW_TYPE_CUBE;
        case TextureViewType::ARRAY_1D:
            return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
        case TextureViewType::ARRAY_2D:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        case TextureViewType::CUBE_ARRAY:
            return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkComponentSwizzle vc::vkComponentSwizzle(const TextureComponentSwizzle& sw) {
    switch (sw) {
        case TextureComponentSwizzle::IDENTITY:
            return VK_COMPONENT_SWIZZLE_IDENTITY;
        case TextureComponentSwizzle::ZERO:
            return VK_COMPONENT_SWIZZLE_ZERO;
        case TextureComponentSwizzle::ONE:
            return VK_COMPONENT_SWIZZLE_ONE;
        case TextureComponentSwizzle::R:
            return VK_COMPONENT_SWIZZLE_R;
        case TextureComponentSwizzle::G:
            return VK_COMPONENT_SWIZZLE_G;
        case TextureComponentSwizzle::B:
            return VK_COMPONENT_SWIZZLE_B;
        case TextureComponentSwizzle::A:
            return VK_COMPONENT_SWIZZLE_A;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkFilter vc::vkFilter(const TextureFilter& filter) {
    switch (filter) {
        case TextureFilter::NEAREST:
            return VK_FILTER_NEAREST;
        case TextureFilter::LINEAR:
            return VK_FILTER_LINEAR;
        case TextureFilter::CUBIC:
            return VK_FILTER_CUBIC_IMG;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkSamplerAddressMode vc::vkSamplerAddressMode(const AddressMode& mode) {
    switch (mode) {
        case AddressMode::REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case AddressMode::MIRRORED_REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case AddressMode::CLAMP_TO_EDGE:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case AddressMode::CLAMP_TO_BORDER:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case AddressMode::MIRROR_CLAMP_TO_EDGE:
            return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkSamplerMipmapMode vc::vkSamplerMipmapMode(const MipmapMode& mipmapMode) {
    switch (mipmapMode) {
        case MipmapMode::NEAREST:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case MipmapMode::LINEAR:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkSamplerCreateInfo vc::vkSamplerCreateInfo(const SamplerCreateInfo& sampler,
                                            float deviceMaxAnisotropic) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = vc::vkFilter(sampler.magnificationFilter);
    samplerInfo.minFilter = vc::vkFilter(sampler.minificationFilter);
    samplerInfo.addressModeU = vc::vkSamplerAddressMode(sampler.uAddressMode);
    samplerInfo.addressModeV = vc::vkSamplerAddressMode(sampler.vAddressMode);
    samplerInfo.addressModeW = vc::vkSamplerAddressMode(sampler.wAddressMode);
    samplerInfo.anisotropyEnable = sampler.anisotropy;
    samplerInfo.unnormalizedCoordinates = !sampler.normalizedCoordinates;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = vc::vkSamplerMipmapMode(sampler.mipmapMode);
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    samplerInfo.maxAnisotropy = sampler.maxAnisotropy < 0.0f
                                ? deviceMaxAnisotropic
                                : sampler.maxAnisotropy;

    return samplerInfo;
}