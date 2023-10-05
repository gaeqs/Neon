//
// Created by gaelr on 28/01/2023.
//

#include "VulkanConversions.h"

#include <stdexcept>

#include <engine/render/TextureCreateInfo.h>
#include <engine/render/FrameBufferTextureCreateInfo.h>
#include <engine/shader/MaterialCreateInfo.h>
#include <vulkan/vulkan_core.h>

namespace vc = neon::vulkan::conversions;

uint32_t vc::pixelSize(const TextureFormat& format) {
    switch (format) {
        case TextureFormat::R32FG32FB32F:
            return 12;
        case TextureFormat::R32FG32FB32FA32F:
            return 16;
        case TextureFormat::R8:
            return 1;
        case TextureFormat::R8G8:
            return 2;
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
        case TextureFormat::R8:
            return VK_FORMAT_R8_UNORM;
        case TextureFormat::R8G8:
            return VK_FORMAT_R8G8_UNORM;
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

        case TextureFormat::R32FG32FB32F:
            return VK_FORMAT_R32G32B32_SFLOAT;
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

std::vector<VkFormat>
vc::vkFormat(const std::vector<FrameBufferTextureCreateInfo>& infos) {
    std::vector<VkFormat> map;
    map.reserve(infos.size());
    for (const auto& item: infos) {
        map.push_back(vkFormat(item.format));
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
                                            float maxLod,
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
    samplerInfo.maxLod = maxLod;

    samplerInfo.maxAnisotropy = sampler.maxAnisotropy < 0.0f
                                ? deviceMaxAnisotropic
                                : sampler.maxAnisotropy;

    return samplerInfo;
}

VkCompareOp vc::vkCompareOp(const DepthCompareOperation& op) {
    switch (op) {
        case DepthCompareOperation::LESS:
            return VK_COMPARE_OP_LESS;
        case DepthCompareOperation::EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case DepthCompareOperation::LESS_OR_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case DepthCompareOperation::GREATER:
            return VK_COMPARE_OP_GREATER;
        case DepthCompareOperation::NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;
        case DepthCompareOperation::GREATER_OR_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case DepthCompareOperation::ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
        case DepthCompareOperation::NEVER:
            return VK_COMPARE_OP_NEVER;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkBlendOp vc::vkBlendOp(const BlendOperation& op) {
    switch (op) {
        case BlendOperation::ADD:
            return VK_BLEND_OP_ADD;
        case BlendOperation::SUBTRACT:
            return VK_BLEND_OP_SUBTRACT;
        case BlendOperation::REVERSE_SUBTRACT:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOperation::MIN:
            return VK_BLEND_OP_MIN;
        case BlendOperation::MAX:
            return VK_BLEND_OP_MAX;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkLogicOp vc::vkLogicOp(const BlendingLogicOperation& op) {
    switch (op) {
        case BlendingLogicOperation::CLEAR:
            return VK_LOGIC_OP_CLEAR;
        case BlendingLogicOperation::AND:
            return VK_LOGIC_OP_AND;
        case BlendingLogicOperation::AND_REVERSE:
            return VK_LOGIC_OP_AND_REVERSE;
        case BlendingLogicOperation::COPY:
            return VK_LOGIC_OP_COPY;
        case BlendingLogicOperation::AND_INVERTED:
            return VK_LOGIC_OP_AND_REVERSE;
        case BlendingLogicOperation::NO_OP:
            return VK_LOGIC_OP_NO_OP;
        case BlendingLogicOperation::XOR:
            return VK_LOGIC_OP_XOR;
        case BlendingLogicOperation::OR:
            return VK_LOGIC_OP_OR;
        case BlendingLogicOperation::NOR:
            return VK_LOGIC_OP_NOR;
        case BlendingLogicOperation::EQUIVALENT:
            return VK_LOGIC_OP_EQUIVALENT;
        case BlendingLogicOperation::INVERT:
            return VK_LOGIC_OP_INVERT;
        case BlendingLogicOperation::OR_REVERSE:
            return VK_LOGIC_OP_OR_REVERSE;
        case BlendingLogicOperation::COPY_INVERTED:
            return VK_LOGIC_OP_COPY_INVERTED;
        case BlendingLogicOperation::OR_INVERTED:
            return VK_LOGIC_OP_OR_INVERTED;
        case BlendingLogicOperation::NAND:
            return VK_LOGIC_OP_NAND;
        case BlendingLogicOperation::SET:
            return VK_LOGIC_OP_SET;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkBlendFactor vc::vkBlendFactor(const BlendFactor& factor) {
    switch (factor) {
        case BlendFactor::ZERO:
            return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::ONE:
            return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SRC_COLOR:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DST_COLOR:
            return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::ONE_MINUS_DST_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DST_ALPHA:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::ONE_MINUS_DST_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendFactor::CONSTANT_COLOR:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::CONSTANT_ALPHA:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SRC_ALPHA_SATURATE:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case BlendFactor::SRC1_COLOR:
            return VK_BLEND_FACTOR_SRC1_COLOR;
        case BlendFactor::ONE_MINUS_SRC1_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::SRC1_ALPHA:
            return VK_BLEND_FACTOR_SRC1_ALPHA;
        case BlendFactor::ONE_MINUS_SRC1_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkPolygonMode vc::vkPolygonMode(const PolygonMode& polygonMode) {
    switch (polygonMode) {
        case PolygonMode::FILL:
            return VK_POLYGON_MODE_FILL;
        case PolygonMode::LINE:
            return VK_POLYGON_MODE_LINE;
        case PolygonMode::POINT:
            return VK_POLYGON_MODE_POINT;
        case PolygonMode::FILL_RECTANGLE_NVIDIA:
            return VK_POLYGON_MODE_FILL_RECTANGLE_NV;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkCullModeFlagBits vc::vkCullModeFlagBits(const CullMode& cullMode) {
    switch (cullMode) {
        case CullMode::NONE:
            return VK_CULL_MODE_NONE;
        case CullMode::FRONT:
            return VK_CULL_MODE_FRONT_BIT;
        case CullMode::BACK:
            return VK_CULL_MODE_BACK_BIT;
        case CullMode::BOTH:
            return VK_CULL_MODE_FRONT_AND_BACK;
        default:
            throw std::runtime_error("Conversion not found!");
    }
}

VkPrimitiveTopology
vc::vkPrimitiveTopology(const neon::PrimitiveTopology& topology) {
    switch (topology) {
        case PrimitiveTopology::POINT_LIST:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case PrimitiveTopology::LINE_LIST:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case PrimitiveTopology::LINE_STRIP:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case PrimitiveTopology::TRIANGLE_STRIP:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case PrimitiveTopology::TRIANGLE_FAN:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        case PrimitiveTopology::LINE_LIST_WITH_ADJACENCY:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
        case PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
        case PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
        case PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
        case PrimitiveTopology::PATCH_LIST:
            return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
        case PrimitiveTopology::TRIANGLE_LIST:
        default:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}
