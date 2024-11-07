//
// Created by gaeqs on 30/10/2024.
//

#include "FrameBufferLoader.h"

#include <neon/render/buffer/SimpleFrameBuffer.h>
#include <neon/render/buffer/SwapChainFrameBuffer.h>

#include "AssetLoaderHelpers.h"
#include "TextureLoader.h"

namespace neon {
    FrameBufferTextureCreateInfo FrameBufferLoader::loadTexture(nlohmann::json& json) {
        FrameBufferTextureCreateInfo info;
        if (!json.is_object()) return info;

        if (auto& name = json["name"]; name.is_string()) {
            info.name = name;
        }

        info.format = serialization::toTextureFormat(json.value("format", "")).value_or(info.format);
        info.layers = json.value("layers", info.layers);
        TextureLoader::loadImageView(json["image_view"], info.imageView);
        TextureLoader::loadSampler(json["sampler"], info.sampler);

        return info;
    }

    std::shared_ptr<FrameBuffer>
    FrameBufferLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) {
        auto& type = json["type"];
        if (!type.is_string()) return nullptr;

        bool depth = json.value("depth", false);

        if (type == "swap_chain") {
            return std::make_shared<SwapChainFrameBuffer>(context.application, name, depth);
        }
        if (type == "simple") {
            std::vector<AssetGeneralProperties<Texture>> props;
            std::vector<FrameBufferTextureCreateInfo> infos;

            auto& textures = json["textures"];
            if (textures.is_object()) {
                props.push_back(fetchGeneralProperties<Texture>(textures, context));
                infos.push_back(loadTexture(textures));
            } else if (textures.is_array()) {
                for (auto& texture: textures) {
                    props.push_back(fetchGeneralProperties<Texture>(texture, context));
                    infos.push_back(loadTexture(texture));
                }
            }

            auto depthProps = fetchGeneralProperties<Texture>(json["depth_properties"], context);
            auto depthName = depthProps.error.has_value() ? std::optional<std::string>{} : depthProps.name;
            auto fb = std::make_shared<SimpleFrameBuffer>(context.application, name, infos, depth, depthName);

            if (depth && !depthProps.error.has_value()) {
                applyGeneralProperties(fb->getTextures().back(), depthProps, context);
            }

            for (size_t i = 0; i < props.size(); ++i) {
                if (props[i].error.has_value()) continue;
                applyGeneralProperties(fb->getTextures()[i], props[i], context);
            }

            return fb;
        }

        return nullptr;
    }
}
