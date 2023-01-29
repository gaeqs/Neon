//
// Created by gaelr on 28/01/2023.
//

#ifndef NEON_VULKANCONVERSIONS_H
#define NEON_VULKANCONVERSIONS_H

#include <vector>

#include <vulkan/vulkan.h>

enum class TextureFormat;
enum class Tiling;
enum class TextureDimensions;
enum class SamplesPerTexel;
enum class TextureViewType;
enum class TextureComponentSwizzle;
enum class TextureFilter;
enum class AddressMode;
enum class MipmapMode;
struct SamplerCreateInfo;

namespace vulkan_conversions {

    uint32_t pixelSize(const TextureFormat& format);

    VkFormat vkFormat(const TextureFormat& format);

    std::vector<VkFormat> vkFormat(const std::vector<TextureFormat>& formats);

    VkImageTiling vkImageTiling(const Tiling& tiling);

    VkImageType vkImageType(const TextureDimensions& dimensions);

    VkSampleCountFlagBits vkSampleCountFlagBits(const SamplesPerTexel& samples);

    VkImageViewType vkImageViewType(const TextureViewType& viewType);

    VkComponentSwizzle vkComponentSwizzle(const TextureComponentSwizzle& sw);

    VkFilter vkFilter(const TextureFilter& filter);

    VkSamplerAddressMode vkSamplerAddressMode(const AddressMode& mode);

    VkSamplerMipmapMode vkSamplerMipmapMode(const MipmapMode& mipmapMode);

    VkSamplerCreateInfo vkSamplerCreateInfo(const SamplerCreateInfo& sampler,
                                            float deviceMaxAnisotropic);
}

#endif //NEON_VULKANCONVERSIONS_H
