//
// Created by gaeqs on 30/10/2024.
//

#include "TextureLoader.h"

#include "AssetLoaderHelpers.h"

namespace neon {
    void TextureLoader::loadImage(nlohmann::json& json, ImageCreateInfo& info) {
        info.format = serialization::toTextureFormat(json["format"]).value_or(info.format);
        info.dimensions = serialization::toTextureDimension(json["dimensions"]).value_or(info.dimensions);
        info.width = json.value("width", info.width);
        info.height = json.value("height", info.height);
        info.depth = json.value("depth", info.depth);
        info.layers = json.value("layers", info.layers);
        info.tiling = serialization::toTiling(json["tiling"]).value_or(info.tiling);
        info.samples = serialization::toSamplesPerTexel(json["samples"]).value_or(info.samples);
        info.mipmaps = json.value("mipmaps", info.mipmaps);

        auto& usages = json["usages"];
        if (usages.is_string()) {
            if (auto optional = serialization::toTextureUsage(usages); optional.has_value()) {
                info.usages = std::vector{optional.value()};
            }
        } else if (usages.is_array()) {
            std::vector<TextureUsage> result;
            for (auto usage: usages) {
                if (auto optional = serialization::toTextureUsage(usages); optional.has_value()) {
                    result.push_back(optional.value());
                }
            }
            info.usages = result;
        }
    }

    void TextureLoader::loadImageView(nlohmann::json& json, ImageViewCreateInfo& info) {
        info.viewType = serialization::toTextureViewType(json["view_type"]).value_or(info.viewType);
        info.rSwizzle = serialization::toTextureComponentSwizzle(json["r_swizzle"]).value_or(info.rSwizzle);
        info.gSwizzle = serialization::toTextureComponentSwizzle(json["g_swizzle"]).value_or(info.gSwizzle);
        info.bSwizzle = serialization::toTextureComponentSwizzle(json["b_swizzle"]).value_or(info.bSwizzle);
        info.aSwizzle = serialization::toTextureComponentSwizzle(json["a_swizzle"]).value_or(info.aSwizzle);
        info.baseMipmapLevel = json.value("base_mipmap_level", info.baseMipmapLevel);
        info.mipmapLevelCount = json.value("mipmap_level_count", info.mipmapLevelCount);
        info.baseArrayLayerLevel = json.value("base_array_layer_level", info.baseArrayLayerLevel);
        info.arrayLayerCount = json.value("array_layer_count", info.arrayLayerCount);
    }

    void TextureLoader::loadSampler(nlohmann::json& json, SamplerCreateInfo& info) {
        info.magnificationFilter = serialization::toTextureFilter(json["magnification_filter"]).value_or(info.magnificationFilter);
        info.minificationFilter = serialization::toTextureFilter(json["minification_filter"]).value_or(info.minificationFilter);
    }

    std::shared_ptr<Texture>
    TextureLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) {
        if (context.fileSystem == nullptr) return nullptr;

        auto source = json["source"];

        std::vector<nlohmann::json> entries;
        if (source.is_array()) {
            entries = source;
        } else {
            entries.push_back(source);
        }

        std::vector<File> files;
        std::vector<const void*> datas;
        std::vector<uint32_t> sizes;

        for (auto& data: entries) {
            if (!data.is_string()) continue;
            auto optional = getFile(std::filesystem::path(data), context);
            if (optional.has_value()) {
                datas.push_back(optional.value().getData());
                sizes.push_back(optional.value().getSize());
                files.push_back(optional.value());
            }
        }

        if (files.empty()) return nullptr;

        TextureCreateInfo info;
        loadImage(json["image"], info.image);
        loadImageView(json["image_view"], info.imageView);

        std::shared_ptr<Texture> texture = std::make_shared<Texture>(context.application, name, )
    }
}
