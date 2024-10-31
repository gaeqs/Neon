//
// Created by gaeqs on 25/10/2024.
//

#include "MaterialLoader.h"

#include <neon/render/Render.h>

#include "AssetLoaderHelpers.h"

namespace neon {
    BlendOperation MaterialLoader::parse(const nlohmann::json& name, BlendOperation def) {
        static const std::array<std::string, 16> OPERATIONS = {
            "add", "subtract", "reverse_subtract", "min", "max"
        };
        if (!name.is_string()) return def;
        auto data = std::find(
            OPERATIONS.begin(),
            OPERATIONS.end(),
            name.get<std::string>()
        );

        if (data == OPERATIONS.end()) return def;
        return static_cast<BlendOperation>(OPERATIONS.begin() - data);
    }

    BlendFactor MaterialLoader::parse(const nlohmann::json& name, BlendFactor def) {
        static const std::array<std::string, 19> FACTORS = {
            "zero", "one", "src_color", "one_minus_src_color", "dst_color",
            "one_minus_dst_color", "src_alpha", "one_minus_src_alpha", "dst_alpha",
            "one_minus_dst_alpha", "constant_color", "one_minus_constant_color",
            "constant_alpha", "one_minus_constant_alpha", "src_alpha_saturate",
            "src1_color", "one_minus_src1_color", "src1_alpha", "one_minus_src1_alpha"
        };
        if (!name.is_string()) return def;
        auto data = std::find(
            FACTORS.begin(),
            FACTORS.end(),
            name.get<std::string>()
        );

        if (data == FACTORS.end()) return def;
        return static_cast<BlendFactor>(FACTORS.begin() - data);
    }

    DepthCompareOperation MaterialLoader::parse(const nlohmann::json& name, DepthCompareOperation def) {
        static const std::array<std::string, 8> OPERATIONS = {
            "never", "less", "equal", "less_or_equal", "greater",
            "not_equal", "greater_or_equal", "always"
        };
        if (!name.is_string()) return def;
        auto data = std::find(
            OPERATIONS.begin(),
            OPERATIONS.end(),
            name.get<std::string>()
        );

        if (data == OPERATIONS.end()) return def;
        return static_cast<DepthCompareOperation>(OPERATIONS.begin() - data);
    }

    PolygonMode MaterialLoader::parse(const nlohmann::json& name, PolygonMode def) {
        static const std::array<std::string, 8> OPERATIONS = {
            "fill", "line", "point", "fill_rectangle_nvidia"
        };
        if (!name.is_string()) return def;
        auto data = std::find(
            OPERATIONS.begin(),
            OPERATIONS.end(),
            name.get<std::string>()
        );

        if (data == OPERATIONS.end()) return def;
        return static_cast<PolygonMode>(OPERATIONS.begin() - data);
    }

    CullMode MaterialLoader::parse(const nlohmann::json& name, CullMode def) {
        static const std::array<std::string, 8> OPERATIONS = {
            "none", "front", "back", "both"
        };
        if (!name.is_string()) return def;
        auto data = std::find(
            OPERATIONS.begin(),
            OPERATIONS.end(),
            name.get<std::string>()
        );

        if (data == OPERATIONS.end()) return def;
        return static_cast<CullMode>(OPERATIONS.begin() - data);
    }

    PrimitiveTopology MaterialLoader::parse(const nlohmann::json& name, PrimitiveTopology def) {
        static const std::array<std::string, 11> OPERATIONS = {
            "point_list", "line_list", "line_strip", "triangle_list", "triangle_strip",
            "triangle_fan", "line_list_with_adjacency", "line_strip_with_adjacency",
            "triangle_list_with_adjacency", "triangle_strip_with_adjacency", "patch_list"
        };
        if (!name.is_string()) return def;
        auto data = std::find(
            OPERATIONS.begin(),
            OPERATIONS.end(),
            name.get<std::string>()
        );

        if (data == OPERATIONS.end()) return def;
        return static_cast<PrimitiveTopology>(OPERATIONS.begin() - data);
    }

    std::vector<InputDescription> MaterialLoader::parse(nlohmann::json& data, InputRate inputRate) {
        std::vector<InputDescription> result;
        if (data.is_null()) return result;

        std::vector<nlohmann::json> elements;
        if (data.is_array()) {
            elements = data;
        } else {
            elements.push_back(data);
        }

        for (auto& entry: elements) {
            if (!entry.is_object()) continue;
            auto stride = entry["stride"];
            if (!stride.is_number_integer()) continue;
            InputDescription description(stride, inputRate);

            auto attributes = entry["attributes"];
            if (attributes.is_array()) {
                for (auto& attribute: attributes) {
                    if (!attribute.is_object()) continue;
                    auto sizeInFloats = attribute["size_in_floats"];
                    auto offsetInBytes = attribute["offset_in_bytes"];
                    if (!sizeInFloats.is_number_integer() || !offsetInBytes.is_number_integer()) continue;
                    description.addAttribute(sizeInFloats, offsetInBytes);
                }
            } else if (attributes.is_object()) {
                if (!attributes.is_object()) continue;
                auto sizeInFloats = attributes["size_in_floats"];
                auto offsetInBytes = attributes["offset_in_bytes"];
                if (!sizeInFloats.is_number_integer() || !offsetInBytes.is_number_integer()) continue;
                description.addAttribute(sizeInFloats, offsetInBytes);
            }

            result.push_back(description);
        }

        return result;
    }

    void MaterialLoader::loadDescriptions(nlohmann::json& json,
                                          MaterialDescriptions& descriptions,
                                          const AssetLoaderContext& context) {
        if (!json.is_object()) return;

        descriptions.vertex = parse(json["vertex"], InputRate::VERTEX);
        descriptions.instance = parse(json["instance"], InputRate::INSTANCE);
        descriptions.uniform = getAsset<ShaderUniformDescriptor>(json["uniform"], context);

        auto extra = json["extra_uniforms"];
        if (extra.is_array()) {
            for (auto& entry: extra) {
                auto result = getAsset<ShaderUniformDescriptor>(entry, context);
                if (result != nullptr) {
                    descriptions.extraUniforms.push_back(result);
                }
            }
        } else {
            auto result = getAsset<ShaderUniformDescriptor>(extra, context);
            if (result != nullptr) {
                descriptions.extraUniforms.push_back(result);
            }
        }
    }

    void MaterialLoader::loadBlending(nlohmann::json& json, MaterialBlending& blending) {
        static const std::array<std::string, 16> LOGIC_OPERATIONS = {
            "clear", "and", "and_reverse", "copy", "and_inverted", "no_op",
            "xor", "or", "nor", "equivalent", "invert",
            "or_reverse", "copy_inverted", "or_inverted", "nand", "set"
        };

        if (!json.is_object()) return;

        blending.logicBlending = json.value("logic_blending", blending.logicBlending);

        if (auto& logicOp = json["logic_operation"]; logicOp.is_string()) {
            auto data = std::find(
                LOGIC_OPERATIONS.begin(),
                LOGIC_OPERATIONS.end(),
                logicOp.get<std::string>()
            );

            if (data != LOGIC_OPERATIONS.end()) {
                blending.logicOperation = static_cast<BlendingLogicOperation>(LOGIC_OPERATIONS.begin() - data);
            }
        }

        if (auto& blendingConstants = json["blending_constants"]; blendingConstants.is_array()) {
            size_t i = 0;
            for (auto entry: blendingConstants) {
                if (blendingConstants.is_number_float()) {
                    blending.blendingConstants[i] = entry;
                }
                ++i;
            }
        }

        if (auto& attachments = json["attachments"]; attachments.is_array()) {
            for (auto& entry: attachments) {
                if (!entry.is_object()) continue;
                MaterialAttachmentBlending attachment;
                attachment.blend = entry.value("enabled", attachment.blend);
                attachment.colorBlendOperation = parse(entry["color_blend_operation"], attachment.colorBlendOperation);
                attachment.colorSourceBlendFactor = parse(entry["color_source_blend_factor"],
                                                          attachment.colorSourceBlendFactor);
                attachment.colorDestinyBlendFactor = parse(entry["color_destiny_blend_factor"],
                                                           attachment.colorDestinyBlendFactor);
                attachment.alphaBlendOperation = parse(entry["alpha_blend_operation"], attachment.alphaBlendOperation);
                attachment.alphaSourceBlendFactor = parse(entry["alpha_source_blend_factor"],
                                                          attachment.alphaSourceBlendFactor);
                attachment.alphaDestinyBlendFactor = parse(entry["alpha_destiny_blend_factor"],
                                                           attachment.alphaDestinyBlendFactor);

                auto& mask = entry["write_mask"];
                if (mask.is_string()) {
                    uint32_t result = 0;
                    for (auto& c: mask.get<std::string>()) {
                        if (c == 'R' || c == 'r') result |= ColorWriteMask::R;
                        else if (c == 'G' || c == 'g') result |= ColorWriteMask::G;
                        else if (c == 'B' || c == 'b') result |= ColorWriteMask::B;
                        else if (c == 'A' || c == 'a') result |= ColorWriteMask::A;
                    }
                    attachment.writeMask = result;
                }

                blending.attachmentsBlending.push_back(attachment);
            }
        }
    }

    void MaterialLoader::loadDepthStencil(nlohmann::json& json, MaterialDepthStencil& depthStencil) {
        if (!json.is_object()) return;
        depthStencil.depthTest = json.value("depth_test", depthStencil.depthTest);
        depthStencil.depthWrite = json.value("depth_write", depthStencil.depthWrite);
        depthStencil.depthCompareOperation = parse(json["depth_compare_operation"], depthStencil.depthCompareOperation);
        depthStencil.useDepthBounds = json.value("use_depth_bounds", depthStencil.useDepthBounds);
        depthStencil.minDepthBounds = json.value("min_depth_bounds", depthStencil.minDepthBounds);
        depthStencil.maxDepthBounds = json.value("max_depth_bounds", depthStencil.maxDepthBounds);
    }

    void MaterialLoader::loadRasterizer(nlohmann::json& json, MaterialRasterizer& rasterizer) {
        rasterizer.polygonMode = parse(json["polygon_mode"], rasterizer.polygonMode);
        rasterizer.lineWidth = json.value("line_width", rasterizer.lineWidth);
        rasterizer.cullMode = parse(json["cull_mode"], rasterizer.cullMode);
        rasterizer.frontFace = json.value<bool>("clockwise", rasterizer.frontFace == FrontFace::CLOCKWISE)
                                   ? FrontFace::CLOCKWISE
                                   : FrontFace::COUNTER_CLOCKWISE;
    }

    std::shared_ptr<Material>
    MaterialLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) {
        auto frameBuffer = neon::getAsset<FrameBuffer>(json["frame_buffer"], context);
        if (frameBuffer == nullptr) return nullptr;

        auto shader = neon::getAsset<ShaderProgram>(json["shader"], context);
        if (shader == nullptr) return nullptr;

        MaterialCreateInfo info(frameBuffer, shader);

        loadDescriptions(json["descriptions"], info.descriptions, context);
        loadBlending(json["blending"], info.blending);
        loadDepthStencil(json["depth_stencil"], info.depthStencil);
        loadRasterizer(json["rasterizer"], info.rasterizer);
        info.topology = parse(json["topology"], info.topology);

        return std::make_shared<Material>(context.application, name, info);
    }
}
