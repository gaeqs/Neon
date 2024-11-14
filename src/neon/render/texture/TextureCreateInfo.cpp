//
// Created by gaelr on 28/01/2023.
//
#include "TextureCreateInfo.h"

#include <algorithm>
#include <unordered_map>

namespace neon::serialization {
    std::optional<TextureFormat> toTextureFormat(std::string s) {
        static const std::unordered_map<std::string, TextureFormat> map = {
            {"R8,", TextureFormat::R8},
            {"R8G8", TextureFormat::R8G8},
            {"R8G8B8", TextureFormat::R8G8B8},
            {"B8G8R8", TextureFormat::B8G8R8},
            {"R8G8B8A8", TextureFormat::R8G8B8A8},
            {"R8G8B8A8_SRGB", TextureFormat::R8G8B8A8_SRGB},
            {"A8R8G8B8", TextureFormat::A8R8G8B8},
            {"B8G8R8A8", TextureFormat::B8G8R8A8},
            {"A8B8G8R8", TextureFormat::A8B8G8R8},
            {"R32FG32FB32F", TextureFormat::R32FG32FB32F},
            {"R32FG32FB32FA32F", TextureFormat::R32FG32FB32FA32F},
            {"R32F", TextureFormat::R32F},
            {"R32FG32F", TextureFormat::R32FG32F},
            {"R16FG16F", TextureFormat::R16FG16F},
            {"R16FG16FB16F", TextureFormat::R16FG16FB16F},
            {"R16FG16FB16FA16F", TextureFormat::R16FG16FB16FA16F},
            {"DEPTH24STENCIL8", TextureFormat::DEPTH24STENCIL8},
        };

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return {};
    }

    std::optional<TextureFilter> toTextureFilter(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });
        if (s == "NEAREST") return TextureFilter::NEAREST;
        if (s == "LINEAR") return TextureFilter::LINEAR;
        if (s == "CUBIC") return TextureFilter::CUBIC;
        return {};
    }

    std::optional<AddressMode> toAddressMode(std::string s) {
        static const std::unordered_map<std::string, AddressMode> map = {
            {"REPEAT", AddressMode::REPEAT},
            {"MIRRORED_REPEAT", AddressMode::MIRRORED_REPEAT},
            {"CLAMP_TO_EDGE", AddressMode::CLAMP_TO_EDGE},
            {"CLAMP_TO_BORDER", AddressMode::CLAMP_TO_BORDER},
            {"MIRROR_CLAMP_TO_EDGE", AddressMode::MIRROR_CLAMP_TO_EDGE},
        };

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return {};
    }

    std::optional<TextureDimensions> toTextureDimension(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });
        if (s == "ONE") return TextureDimensions::ONE;
        if (s == "TWO") return TextureDimensions::TWO;
        if (s == "THREE") return TextureDimensions::THREE;
        return {};
    }

    std::optional<TextureUsage> toTextureUsage(std::string s) {
        static const std::unordered_map<std::string, TextureUsage> map = {
            {"TRANSFER_SOURCE", TextureUsage::TRANSFER_SOURCE},
            {"TRANSFER_DESTINY", TextureUsage::TRANSFER_DESTINY},
            {"SAMPLING", TextureUsage::SAMPLING},
            {"STORAGE", TextureUsage::STORAGE},
            {"COLOR_ATTACHMENT", TextureUsage::COLOR_ATTACHMENT},
            {"DEPTH_STENCIL_ATTACHMENT", TextureUsage::DEPTH_STENCIL_ATTACHMENT},
            {"TRANSFER_ATTACHMENT", TextureUsage::TRANSFER_ATTACHMENT},
            {"INPUT_ATTACHMENT", TextureUsage::INPUT_ATTACHMENT},
            {"VIDEO_DECODE_DESTINY", TextureUsage::VIDEO_DECODE_DESTINY},
            {"VIDEO_DECODE_SOURCE", TextureUsage::VIDEO_DECODE_SOURCE},
            {"VIDEO_DECODE_DPB", TextureUsage::VIDEO_DECODE_DPB},
            {"FRAGMENT_DESTINY_MAP", TextureUsage::FRAGMENT_DESTINY_MAP},
            {"FRAGMENT_SHADING_RATE_ATTACHMENT", TextureUsage::FRAGMENT_SHADING_RATE_ATTACHMENT},
            {"ATTACHMENT_FEEDBACK_LOOP", TextureUsage::ATTACHMENT_FEEDBACK_LOOP},
            {"INVOCATION_MARK", TextureUsage::INVOCATION_MARK},
            {"SAMPLE_WEIGHT", TextureUsage::SAMPLE_WEIGHT},
            {"SAMPLE_BLOCK_MATCH", TextureUsage::SAMPLE_BLOCK_MATCH},
        };

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return {};
    }

    std::optional<TextureViewType> toTextureViewType(std::string s) {
        static const std::unordered_map<std::string, TextureViewType> map = {
            {"NORMAL_1D", TextureViewType::NORMAL_1D},
            {"NORMAL_2D", TextureViewType::NORMAL_2D},
            {"NORMAL_3D", TextureViewType::NORMAL_3D},
            {"CUBE", TextureViewType::CUBE},
            {"ARRAY_1D", TextureViewType::ARRAY_1D},
            {"ARRAY_2D", TextureViewType::ARRAY_2D},
            {"CUBE_ARRAY", TextureViewType::CUBE_ARRAY}
        };

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return {};
    }

    std::optional<SamplesPerTexel> toSamplesPerTexel(std::string s) {
        static const std::unordered_map<std::string, SamplesPerTexel> map = {
            {"COUNT_1", SamplesPerTexel::COUNT_1},
            {"COUNT_2", SamplesPerTexel::COUNT_2},
            {"COUNT_4", SamplesPerTexel::COUNT_4},
            {"COUNT_8", SamplesPerTexel::COUNT_8},
            {"COUNT_16", SamplesPerTexel::COUNT_16},
            {"COUNT_32", SamplesPerTexel::COUNT_32},
            {"COUNT_64", SamplesPerTexel::COUNT_64},
        };

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return {};
    }

    std::optional<Tiling> toTiling(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });
        if (s == "OPTIMAL") return Tiling::OPTIMAL;
        if (s == "LINEAR") return Tiling::LINEAR;
        if (s == "DRM") return Tiling::DRM;
        return {};
    }

    std::optional<TextureComponentSwizzle> toTextureComponentSwizzle(std::string s) {
        static const std::unordered_map<std::string, TextureComponentSwizzle> map = {
            {"IDENTITY", TextureComponentSwizzle::IDENTITY},
            {"ZERO", TextureComponentSwizzle::ZERO},
            {"ONE", TextureComponentSwizzle::ONE},
            {"R", TextureComponentSwizzle::R},
            {"G", TextureComponentSwizzle::G},
            {"B", TextureComponentSwizzle::B},
            {"A", TextureComponentSwizzle::A},
        };

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return {};
    }

    std::optional<MipmapMode> toMipmapMode(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });
        if (s == "NEAREST") return MipmapMode::NEAREST;
        if (s == "LINEAR") return MipmapMode::LINEAR;
        return {};
    }
}
