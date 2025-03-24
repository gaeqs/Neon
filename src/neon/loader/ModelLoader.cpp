//
// Created by gaeqs on 5/11/24.
//

#include "ModelLoader.h"

#include <neon/assimp/AssimpScene.h>
#include <neon/render/model/Mesh.h>

#include "AssetLoaderHelpers.h"

namespace neon
{
    size_t ModelLoader::getEntrySizeInFloats(LocalVertexEntry entry)
    {
        switch (entry) {
            case LocalVertexEntry::POSITION:
            case LocalVertexEntry::NORMAL:
            case LocalVertexEntry::TANGENT:
                return 3;
            case LocalVertexEntry::UV:
                return 2;
            case LocalVertexEntry::COLOR:
                return 4;
            default:
                return 0;
        }
    }

    std::shared_ptr<Material> ModelLoader::loadMaterial(nlohmann::json& metadata, const AssimpMaterial& assimpMaterial,
                                                        const AssetLoaderContext& context)
    {
        auto material = getAsset<Material>(metadata["material"], context);
        if (material == nullptr) {
            return nullptr;
        }

        auto& textures = metadata["textures"];
        if (textures.is_object()) {
            for (auto& [key, value] : textures.items()) {
                if (!value.is_string()) {
                    continue;
                }
                auto type = serialization::toAssimpTextureType(value);
                if (!type.has_value()) {
                    continue;
                }
                auto texture = assimpMaterial.getTextures().find(type.value());
                if (texture == assimpMaterial.getTextures().end()) {
                    continue;
                }
                material->setTexture(key, texture->second);
            }
        }

        return material;
    }

    std::shared_ptr<Mesh> ModelLoader::loadMesh(nlohmann::json& metadata, const LocalMesh& localMesh,
                                                const std::vector<std::shared_ptr<Material>>& materials,
                                                const AssetLoaderContext& context)
    {
        auto& input = metadata["input"];

        std::vector<LocalVertexEntry> entries;
        size_t sizeInFloats = 0;

        if (input.is_array()) {
            for (auto& i : input) {
                if (i.is_string()) {
                    auto result = serialization::toLocalVertexEntry(i);
                    if (result.has_value()) {
                        entries.push_back(result.value());
                        sizeInFloats += getEntrySizeInFloats(result.value());
                    }
                }
            }
        }

        size_t p = 0;
        auto buffer = std::vector<float>(sizeInFloats * localMesh.getData().size());

        for (const auto& [position, normal, tangent, uv, color, extra] : localMesh.getData()) {
            for (auto& entry : entries) {
                switch (entry) {
                    case LocalVertexEntry::POSITION:
                        buffer[p++] = position[0];
                        buffer[p++] = position[1];
                        buffer[p++] = position[2];
                        break;
                    case LocalVertexEntry::NORMAL:
                        buffer[p++] = normal[0];
                        buffer[p++] = normal[1];
                        buffer[p++] = normal[2];
                        break;
                    case LocalVertexEntry::TANGENT:
                        buffer[p++] = tangent[0];
                        buffer[p++] = tangent[1];
                        buffer[p++] = tangent[2];
                        break;
                    case LocalVertexEntry::UV:
                        buffer[p++] = uv[0];
                        buffer[p++] = uv[1];
                        break;
                    case LocalVertexEntry::COLOR:
                        buffer[p++] = color[0];
                        buffer[p++] = color[1];
                        buffer[p++] = color[2];
                        buffer[p++] = color[3];
                        break;
                }
            }
        }

        std::shared_ptr<Material> material = nullptr;
        if (localMesh.getMaterialIndex() < materials.size()) {
            material = materials[localMesh.getMaterialIndex()];
        }

        auto mesh = std::make_shared<Mesh>(context.application, "local_mesh", material);
        mesh->uploadVertices(buffer);
        mesh->uploadIndices(localMesh.getIndices());

        return mesh;
    }

    void ModelLoader::applyAssimpModel(nlohmann::json& metadata, const std::shared_ptr<AssimpScene>& scene,
                                       ModelCreateInfo& info, const AssetLoaderContext& context)
    {
        // MATERIALS
        auto& jsonMaterials = metadata["materials"];
        std::vector<nlohmann::json> materialsMetadata;
        if (jsonMaterials.is_array()) {
            materialsMetadata = jsonMaterials;
        } else if (jsonMaterials.is_object()) {
            materialsMetadata.push_back(jsonMaterials);
        }

        std::vector<std::shared_ptr<Material>> materials;

        if (!materialsMetadata.empty()) {
            materialsMetadata.resize(scene->getMaterials().size(), materialsMetadata.front());

            size_t i = 0;
            for (const auto& material : scene->getMaterials()) {
                materials.push_back(loadMaterial(materialsMetadata[i++], material, context));
            }
        }

        // MESHES
        auto& jsonMeshes = metadata["meshes"];

        std::vector<nlohmann::json> meshesMetadata;
        if (jsonMeshes.is_array()) {
            meshesMetadata = jsonMeshes;
        } else if (jsonMeshes.is_object()) {
            meshesMetadata.push_back(jsonMeshes);
        }

        std::vector<std::shared_ptr<Mesh>> meshes;
        if (!meshesMetadata.empty()) {
            meshesMetadata.resize(scene->getLocalModel()->getMeshes().size(), meshesMetadata.front());

            size_t i = 0;
            for (const auto& mesh : scene->getLocalModel()->getMeshes()) {
                auto result = loadMesh(meshesMetadata[i++], mesh, materials, context);
                if (result != nullptr) {
                    meshes.push_back(result);
                    info.drawables.push_back(result);
                }
            }
        }

        // EXTRA MATERIALS

        auto& jsonExtraMaterials = metadata["extra_materials"];
        std::vector<nlohmann::json> extraMaterialsEntries;
        if (jsonExtraMaterials.is_array()) {
            extraMaterialsEntries = jsonExtraMaterials;
        } else {
            extraMaterialsEntries.push_back(jsonExtraMaterials);
        }

        for (auto& entry : extraMaterialsEntries) {
            auto material = getAsset<Material>(entry, context);
            if (material != nullptr) {
                for (auto& mesh : meshes) {
                    mesh->getMaterials().insert(material);
                }
            }
        }
    }

    std::shared_ptr<Model> ModelLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context)
    {
        ModelCreateInfo info;
        info.maximumInstances = json.value("maximum_instances", info.maximumInstances);
        info.uniformDescriptor = getAsset<ShaderUniformDescriptor>(json["uniform_descriptor"], context);
        info.shouldAutoFlush = json.value("auto_flush", info.shouldAutoFlush);

        auto& instanceTypes = json["custom_instance_sizes"];
        if (instanceTypes.is_array()) {
            std::vector<size_t> sizes;
            for (auto& entry : instanceTypes) {
                if (entry.is_number_unsigned()) {
                    sizes.push_back(entry.get<size_t>());
                }
            }

            info.instanceSizes = sizes;
            info.instanceTypes.clear();
            for (size_t _ : info.instanceSizes) {
                info.instanceTypes.push_back(typeid(void));
            }
        }

        if (auto& meshes = json["meshes"]; meshes.is_array()) {
            for (auto& entry : meshes) {
                if (auto mesh = getAsset<Mesh>(entry, context); mesh != nullptr) {
                    info.drawables.push_back(mesh);
                }
            }
        }

        auto assimp = getAsset<AssimpScene>(json["assimp"], context);
        if (assimp != nullptr) {
            applyAssimpModel(json["assimp_metadata"], assimp, info, context);
        }

        return std::make_shared<Model>(context.application, name, info);
    }
} // namespace neon
