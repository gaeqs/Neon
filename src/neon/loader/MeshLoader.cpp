//
// Created by gaeqs on 3/11/24.
//

#include "MeshLoader.h"

#include "AssetLoaderHelpers.h"

namespace neon
{
    std::unordered_set<std::shared_ptr<Material>> MeshLoader::loadMaterials(nlohmann::json& json,
                                                                            const AssetLoaderContext& context)
    {
        std::unordered_set<std::shared_ptr<Material>> materials;
        std::vector<nlohmann::json> rawMaterials;

        if (json.is_array()) {
            rawMaterials = json;
        } else {
            rawMaterials.push_back(json);
        }

        for (auto& entry : json) {
            if (auto material = getAsset<Material>(entry, context); material != nullptr) {
                materials.insert(material);
            }
        }

        return materials;
    }

    std::vector<float> MeshLoader::loadVerticesData(nlohmann::json& json)
    {
        if (!json.is_array()) {
            return {};
        }

        std::vector<float> vertices;

        for (auto& entry : json) {
            if (!entry.is_number_float()) {
                continue;
            }
            vertices.push_back(entry.get<float>());
        }

        return vertices;
    }

    std::vector<uint32_t> MeshLoader::loadIndices(nlohmann::json& json)
    {
        if (!json.is_array()) {
            return {};
        }

        std::vector<uint32_t> indices;

        for (auto& entry : json) {
            if (!entry.is_number_unsigned()) {
                continue;
            }
            indices.push_back(entry.get<uint32_t>());
        }

        return indices;
    }

    std::shared_ptr<Mesh> MeshLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context)
    {
        bool modifiableVertices = json.value("modifiable_vertices", false);
        bool modifiableIndices = json.value("modifiable_indices", false);

        auto materials = loadMaterials(json["materials"], context);

        auto mesh = std::make_shared<Mesh>(context.application, name, materials, modifiableVertices, modifiableIndices);

        mesh->uploadVertices(loadVerticesData(json["vertices"]));
        mesh->uploadIndices(loadIndices(json["indices"]));

        return mesh;
    }
} // namespace neon
