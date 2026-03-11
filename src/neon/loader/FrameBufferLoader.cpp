//
// Created by gaeqs on 30/10/2024.
//

#include "FrameBufferLoader.h"

#include <neon/render/buffer/SimpleFrameBuffer.h>
#include <neon/render/buffer/SwapChainFrameBuffer.h>

#include "AssetLoaderHelpers.h"
#include "TextureLoader.h"

namespace neon
{
    FrameBufferTextureCreateInfo FrameBufferLoader::loadTexture(nlohmann::json& json)
    {
        FrameBufferTextureCreateInfo info;
        if (!json.is_object()) {
            return info;
        }

        if (auto& name = json["name"]; name.is_string()) {
            info.name = name;
        }

        if (auto& resolved = json["resolved"]; resolved.is_object()) {
            if (auto& resolvedName = resolved["name"]; resolvedName.is_string()) {
                info.resolveName = resolvedName;
            }
        }

        info.format = serialization::toTextureFormat(json.value("format", "")).value_or(info.format);
        info.layers = json.value("layers", info.layers);
        TextureLoader::loadImageView(json["image_view"], info.imageView);
        TextureLoader::loadSampler(json["sampler"], info.sampler);

        return info;
    }

    std::shared_ptr<FrameBuffer> FrameBufferLoader::loadAsset(std::string name, nlohmann::json json,
                                                              AssetLoaderContext context)
    {
        auto& type = json["type"];
        if (!type.is_string()) {
            return nullptr;
        }

        bool depth = json.value("depth", false);

        auto samples = serialization::toSamplesPerTexel(json.value("samples", "")).value_or(SamplesPerTexel::COUNT_1);

        if (type == "swap_chain") {
            return std::make_shared<SwapChainFrameBuffer>(context.application, name, samples, depth);
        }
        if (type == "simple") {
            std::vector<std::pair<AssetGeneralProperties<SampledTexture>, AssetGeneralProperties<SampledTexture>>>
                props;
            std::vector<FrameBufferTextureCreateInfo> infos;

            auto& textures = json["textures"];
            if (textures.is_object()) {
                auto original = fetchGeneralProperties<SampledTexture>(textures, context);
                auto resolved = fetchGeneralProperties<SampledTexture>(textures["resolved"], context);
                props.emplace_back(original, resolved);
                infos.push_back(loadTexture(textures));
            } else if (textures.is_array()) {
                for (auto& texture : textures) {
                    auto original = fetchGeneralProperties<SampledTexture>(texture, context);
                    auto resolved = fetchGeneralProperties<SampledTexture>(texture["resolved"], context);
                    props.emplace_back(original, resolved);
                    infos.push_back(loadTexture(texture));
                }
            }

            auto& depthJson = json["depth_properties"];

            auto depthProps = fetchGeneralProperties<SampledTexture>(depthJson, context);
            auto depthName = depthProps.error.has_value() ? std::optional<std::string>{} : depthProps.name;

            std::optional<FrameBufferDepthCreateInfo> depthInfo = {};
            if (depth) {
                depthInfo = FrameBufferDepthCreateInfo();
                depthInfo->name = depthName;
            }

            auto fb = std::make_shared<SimpleFrameBuffer>(context.application, name, samples, infos, depthInfo);

            auto outputs = fb->getOutputs();
            for (size_t i = 0; i < outputs.size(); ++i) {
                auto& output = outputs[i];

                if (output.type == FrameBufferOutputType::DEPTH) {
                    if (!depthProps.error.has_value()) {
                        applyGeneralProperties(output.sampled, depthProps, context);
                    }
                    if (!depthProps.error.has_value() && output.texture != output.resolvedTexture) {
                        applyGeneralProperties(output.resolvedSampled, depthProps, context);
                    }
                } else {
                    auto& prop = props[i];

                    if (!prop.first.error.has_value()) {
                        applyGeneralProperties(output.sampled, prop.first, context);
                    }
                    if (!prop.second.error.has_value() && output.texture != output.resolvedTexture) {
                        applyGeneralProperties(output.resolvedSampled, prop.second, context);
                    }
                }
            }

            return fb;
        }

        return nullptr;
    }
} // namespace neon
