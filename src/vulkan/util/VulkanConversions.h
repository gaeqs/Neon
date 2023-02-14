//
// Created by gaelr on 28/01/2023.
//

#ifndef NEON_VULKANCONVERSIONS_H
#define NEON_VULKANCONVERSIONS_H

#include "engine/shader/MaterialCreateInfo.h"
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

enum class TextureFormat;
enum class Tiling;
enum class TextureDimensions;
enum class SamplesPerTexel;
enum class TextureViewType;
enum class TextureComponentSwizzle;
enum class TextureFilter;
enum class AddressMode;
enum class MipmapMode;
enum class DepthCompareOperation;
enum class BlendOperation;
enum class BlendFactor;
enum class BlendingLogicOperation;
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

    VkCompareOp vkCompareOp(const DepthCompareOperation& op);

    VkBlendOp vkBlendOp(const BlendOperation& op);

    VkLogicOp vkLogicOp (const BlendingLogicOperation& op);

    VkBlendFactor vkBlendFactor(const BlendFactor& factor);
}

#endif //NEON_VULKANCONVERSIONS_H
