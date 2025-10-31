//
// Created by gaeqs on 21/04/2025.
//

#include "ShaderUniformBufferLoader.h"

#include "AssetLoaderHelpers.h"

namespace neon
{
    void ShaderUniformBufferLoader::loadData(const std::shared_ptr<ShaderUniformBuffer>& buffer,
                                             AssetLoaderContext context, nlohmann::json& json)
    {
        if (!json.is_object()) {
            neon::error() << context.path.value_or("") << ": entry is not an object.";
            return;
        }

        auto& indexJson = json["index"];
        if (!indexJson.is_number_unsigned()) {
            neon::error() << context.path.value_or("") << ": invalid index.";
            return;
        }

        auto& typeJson = json["type"];
        if (!typeJson.is_string()) {
            neon::error() << context.path.value_or("") << ": invalid type.";
            return;
        }
        std::string type = typeJson;
        std::ranges::transform(type, type.begin(), [](unsigned char c) { return std::toupper(c); });
        if (type == "IMAGE") {
            auto texture = getAsset<SampledTexture>(json["texture"], context);
            if (texture == nullptr) {
                neon::error() << context.path.value_or("") << ": texture not found. " << json.dump();
                return;
            }
            buffer->setTexture(indexJson, texture);
        } else if (type == "BUFFER") {
            // Data in uint_t format.
            auto& dataJson = json["data"];
            if (!dataJson.is_array()) {
                neon::error() << context.path.value_or("") << ": data is not an array.";
                return;
            }
            std::vector<uint8_t> data;
            data.resize(dataJson.size());
            for (const auto& entry : dataJson) {
                if (!entry.is_number_unsigned()) {
                    neon::error() << context.path.value_or("") << ": invalid data index" << entry << ".";
                    return;
                }
                data.push_back(entry.get<uint8_t>());
            }

            buffer->uploadData(indexJson, data.data(), data.size());
        } else {
            neon::error() << context.path.value_or("") << ": invalid type.";
        }
    }

    std::shared_ptr<ShaderUniformBuffer> ShaderUniformBufferLoader::loadAsset(std::string name, nlohmann::json json,
                                                                              AssetLoaderContext context)
    {
        auto descriptor = getAsset<ShaderUniformDescriptor>(json["descriptor"], context);
        if (descriptor == nullptr) {
            return nullptr;
        }

        auto buffer = std::make_shared<ShaderUniformBuffer>(name, descriptor);

        auto& data = json["data"];
        if (data.is_object()) {
            loadData(buffer, context, data);
        } else if (data.is_array()) {
            for (auto entry : data) {
                loadData(buffer, context, entry);
            }
        }
        return buffer;
    }
} // namespace neon