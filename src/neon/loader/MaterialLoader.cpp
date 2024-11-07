//
// Created by gaeqs on 25/10/2024.
//

#include "MaterialLoader.h"

#include <neon/render/Render.h>

#include "AssetLoaderHelpers.h"

namespace neon {
    BlendOperation MaterialLoader::parse(const nlohmann::json& name, BlendOperation def) {
        static const std::unordered_map<std::string, BlendOperation> map = {
            {"ADD", BlendOperation::ADD},
            {"SUBTRACT", BlendOperation::SUBTRACT},
            {"REVERSE_SUBTRACT", BlendOperation::REVERSE_SUBTRACT},
            {"MIN", BlendOperation::MIN},
            {"MAX", BlendOperation::MAX}
        };

        if(!name.is_string()) return def;
        std::string s = name;

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return def;
    }

    BlendFactor MaterialLoader::parse(const nlohmann::json& name, BlendFactor def) {
        static const std::unordered_map<std::string, BlendFactor> map = {
            {"ZERO", BlendFactor::ZERO},
            {"ONE", BlendFactor::ONE},
            {"SRC_COLOR", BlendFactor::SRC_COLOR},
            {"ONE_MINUS_SRC_COLOR", BlendFactor::ONE_MINUS_SRC_COLOR},
            {"DST_COLOR", BlendFactor::DST_COLOR},
            {"ONE_MINUS_DST_COLOR", BlendFactor::ONE_MINUS_DST_COLOR},
            {"SRC_ALPHA", BlendFactor::SRC_ALPHA},
            {"ONE_MINUS_SRC_ALPHA", BlendFactor::ONE_MINUS_SRC_ALPHA},
            {"DST_ALPHA", BlendFactor::DST_ALPHA},
            {"ONE_MINUS_DST_ALPHA", BlendFactor::ONE_MINUS_DST_ALPHA},
            {"CONSTANT_COLOR", BlendFactor::CONSTANT_COLOR},
            {"ONE_MINUS_CONSTANT_COLOR", BlendFactor::ONE_MINUS_CONSTANT_COLOR},
            {"CONSTANT_ALPHA", BlendFactor::CONSTANT_ALPHA},
            {"ONE_MINUS_CONSTANT_ALPHA", BlendFactor::ONE_MINUS_CONSTANT_ALPHA},
            {"SRC_ALPHA_SATURATE", BlendFactor::SRC_ALPHA_SATURATE},
            {"SRC1_COLOR", BlendFactor::SRC1_COLOR},
            {"ONE_MINUS_SRC1_COLOR", BlendFactor::ONE_MINUS_SRC1_COLOR},
            {"SRC1_ALPHA", BlendFactor::SRC1_ALPHA},
            {"ONE_MINUS_SRC1_ALPHA", BlendFactor::ONE_MINUS_SRC1_ALPHA},
        };

        if(!name.is_string()) return def;
        std::string s = name;

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return def;
    }

    DepthCompareOperation MaterialLoader::parse(const nlohmann::json& name, DepthCompareOperation def) {
        static const std::unordered_map<std::string, DepthCompareOperation> map = {
            {"NEVER", DepthCompareOperation::NEVER},
            {"LESS", DepthCompareOperation::LESS},
            {"EQUAL", DepthCompareOperation::EQUAL},
            {"LESS_OR_EQUAL", DepthCompareOperation::LESS_OR_EQUAL},
            {"GREATER", DepthCompareOperation::GREATER},
            {"NOT_EQUAL", DepthCompareOperation::NOT_EQUAL},
            {"GREATER_OR_EQUAL", DepthCompareOperation::GREATER_OR_EQUAL},
            {"ALWAYS", DepthCompareOperation::ALWAYS},
        };

        if(!name.is_string()) return def;
        std::string s = name;

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return def;
    }

    PolygonMode MaterialLoader::parse(const nlohmann::json& name, PolygonMode def) {
        static const std::unordered_map<std::string, PolygonMode> map = {
            {"FILL", PolygonMode::FILL},
            {"LINE", PolygonMode::LINE},
            {"POINT", PolygonMode::POINT},
            {"FILL_RECTANGLE_NVIDIA", PolygonMode::FILL_RECTANGLE_NVIDIA}
        };

        if(!name.is_string()) return def;
        std::string s = name;

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return def;
    }

    CullMode MaterialLoader::parse(const nlohmann::json& name, CullMode def) {
        static const std::unordered_map<std::string, CullMode> map = {
            {"NONE", CullMode::NONE},
            {"FRONT", CullMode::FRONT},
            {"BACK", CullMode::BACK},
            {"BOTH", CullMode::BOTH}
        };

        if(!name.is_string()) return def;
        std::string s = name;

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return def;
    }

    PrimitiveTopology MaterialLoader::parse(const nlohmann::json& name, PrimitiveTopology def) {
        static const std::unordered_map<std::string, PrimitiveTopology> map = {
            {"POINT_LIST", PrimitiveTopology::POINT_LIST},
            {"LINE_LIST", PrimitiveTopology::LINE_LIST},
            {"LINE_STRIP", PrimitiveTopology::LINE_STRIP},
            {"TRIANGLE_LIST", PrimitiveTopology::TRIANGLE_LIST},
            {"TRIANGLE_STRIP", PrimitiveTopology::TRIANGLE_STRIP},
            {"TRIANGLE_FAN", PrimitiveTopology::TRIANGLE_FAN},
            {"LINE_LIST_WITH_ADJACENCY", PrimitiveTopology::LINE_LIST_WITH_ADJACENCY},
            {"LINE_STRIP_WITH_ADJACENCY", PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY},
            {"TRIANGLE_LIST_WITH_ADJACENCY", PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY},
            {"TRIANGLE_STRIP_WITH_ADJACENCY", PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY},
            {"PATCH_LIST", PrimitiveTopology::PATCH_LIST}
        };

        if(!name.is_string()) return def;
        std::string s = name;

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return def;
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
                if (i >= 4) break;
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
        if (frameBuffer == nullptr) {
            Logger::defaultLogger()->warning(MessageBuilder()
                .print("Cannot load material ")
                .print(name)
                .print(". Frame buffer not found."));
            return nullptr;
        }

        auto shader = neon::getAsset<ShaderProgram>(json["shader"], context);
        if (shader == nullptr) {
            Logger::defaultLogger()->warning(MessageBuilder()
                .print("Cannot load material ")
                .print(name)
                .print(". Shader not found."));
            return nullptr;
        }

        MaterialCreateInfo info(frameBuffer, shader);

        loadDescriptions(json["descriptions"], info.descriptions, context);
        loadBlending(json["blending"], info.blending);
        loadDepthStencil(json["depth_stencil"], info.depthStencil);
        loadRasterizer(json["rasterizer"], info.rasterizer);
        info.topology = parse(json["topology"], info.topology);

        auto material = std::make_shared<Material>(context.application, name, info);
        material->setPriority(json.value("priority", material->getPriority()));
        return material;
    }
}
