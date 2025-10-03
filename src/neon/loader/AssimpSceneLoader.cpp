//
// Created by gaeqs on 4/11/24.
//

#include "AssimpSceneLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <neon/assimp/AssimpGeometry.h>
#include <neon/assimp/AssimpNewIOSystem.h>

namespace
{
    rush::Vec2f toRush(aiVector2D v)
    {
        return {v.x, v.y};
    }

    rush::Vec3f toRush(aiVector3D v)
    {
        return {v.x, v.y, v.z};
    }

    rush::Vec4f toRush(aiColor4D c)
    {
        return {c.r, c.g, c.b, c.a};
    }
} // namespace

namespace neon
{
    uint32_t AssimpSceneLoader::decodeFlags(const nlohmann::json& json)
    {
        bool flipUVs = json.value("flip_uvs", false);
        bool flipWindingOrder = json.value("flip_winding_order", false);

        uint32_t flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType |
                         aiProcess_RemoveRedundantMaterials | aiProcess_EmbedTextures;
        if (flipUVs) {
            flags |= aiProcess_FlipUVs;
        }
        if (flipWindingOrder) {
            flags |= aiProcess_FlipWindingOrder;
        }
        return flags;
    }

    std::vector<std::shared_ptr<Texture>> AssimpSceneLoader::loadTextures(const aiScene* scene,
                                                                          const AssetLoaderContext& context)
    {
        std::vector<std::shared_ptr<Texture>> textures;
        for (size_t i = 0; i < scene->mNumTextures; ++i) {
            aiTexture* texture = scene->mTextures[i];
            auto name = "*" + std::to_string(i);

            TextureCreateInfo info;
            if (texture->mHeight == 0) {
                // Compressed texture
                textures.push_back(Texture::createTextureFromFile(context.application, name, texture->pcData,
                                                                  texture->mWidth, info, context.commandBuffer));
            } else {
                info.width = texture->mWidth;
                info.height = texture->mHeight;
                info.depth = 1;
                info.format = TextureFormat::A8R8G8B8;
                textures.push_back(Texture::createFromRawData(context.application, name, texture->pcData, info,
                                                              context.commandBuffer));
            }
        }

        return textures;
    }

    std::vector<AssimpMaterial> AssimpSceneLoader::loadMaterials(const aiScene* scene,
                                                                 const std::vector<std::shared_ptr<Texture>>& textures)
    {
        std::vector<AssimpMaterial> materials;
        for (size_t iMat = 0; iMat < scene->mNumMaterials; ++iMat) {
            auto* aiMat = scene->mMaterials[iMat];

            std::unordered_map<std::string, AssimpMaterialProperty> matProperties;
            std::unordered_map<AssimpMaterialTextureType, std::shared_ptr<Texture>> matTextures;

            for (size_t iProp = 0; iProp < aiMat->mNumProperties; ++iProp) {
                auto* aiProp = aiMat->mProperties[iProp];
                char* data = aiProp->mData;
                size_t length = aiProp->mDataLength;
                if (aiProp->mType == aiPTI_String) {
                    data += 4;

                    length -= 5;
                }

                std::string key = std::string(aiProp->mKey.C_Str());

                AssimpMaterialProperty prop(data, length, static_cast<AssimpMaterialPropertyType>(aiProp->mType));
                if (key == _AI_MATKEY_TEXTURE_BASE) {
                    // Texture! We have to find it!
                    std::string name = prop.asString();
                    auto it = std::find_if(
                        textures.begin(), textures.end(),
                        [&name](const std::shared_ptr<Texture>& texture) { return texture->getName() == name; });
                    if (it != textures.end()) {
                        matTextures[static_cast<AssimpMaterialTextureType>(aiProp->mSemantic)] = *it;
                    }
                } else {
                    matProperties.insert({key, prop});
                }
            }

            materials.emplace_back(aiMat->GetName().C_Str(), matProperties, matTextures);
        }
        return materials;
    }

    std::shared_ptr<LocalModel> AssimpSceneLoader::loadModel(std::string name, const aiScene* scene)
    {
        std::vector<LocalMesh> meshes;

        for (size_t iMesh = 0; iMesh < scene->mNumMeshes; ++iMesh) {
            aiMesh* mesh = scene->mMeshes[iMesh];

            LocalVertexProperties properties;
            properties.hasPosition = mesh->HasPositions();
            properties.hasNormal = mesh->HasNormals();
            properties.hasTangent = properties.hasNormal;
            properties.hasColor = mesh->HasVertexColors(0);
            properties.hasUv = mesh->HasTextureCoords(0);
            properties.hasExtra = false;

            std::vector<rush::Vec3f> tangents =
                properties.hasTangent ? assimp_geometry::calculateTangents(mesh) : std::vector<rush::Vec3f>();

            std::vector<LocalVertex> vertices;
            vertices.reserve(mesh->mNumVertices);

            for (size_t i = 0; i < mesh->mNumVertices; ++i) {
                LocalVertex vertex;
                if (properties.hasPosition) {
                    vertex.position = toRush(mesh->mVertices[i]);
                }
                if (properties.hasNormal) {
                    vertex.normal = toRush(mesh->mNormals[i]);
                }
                if (properties.hasTangent) {
                    vertex.tangent = tangents[i];
                }
                if (properties.hasColor) {
                    vertex.color = toRush(mesh->mColors[0][i]);
                }
                if (properties.hasUv) {
                    vertex.uv = toRush(mesh->mTextureCoords[0][i]);
                }
                vertices.push_back(vertex);
            }

            std::vector<uint32_t> indices;
            indices.reserve(mesh->mNumFaces * 3);
            for (size_t i = 0; i < mesh->mNumFaces; ++i) {
                auto face = mesh->mFaces[i];
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }

            meshes.emplace_back(properties, std::move(vertices), std::move(indices), mesh->mMaterialIndex);
        }

        return std::make_shared<LocalModel>(name, meshes);
    }

    std::shared_ptr<AssimpScene> AssimpSceneLoader::loadAsset(std::string name, nlohmann::json json,
                                                              AssetLoaderContext context)
    {
        auto file = json["file"];
        if (!file.is_string()) {
            return nullptr;
        }
        auto relative = std::filesystem::path(file.get<std::string>());
        auto absolute = context.path.has_value() ? context.path.value().parent_path() / relative : relative;

        Assimp::Importer importer;
        importer.SetIOHandler(new assimp_loader::AssimpNewIOSystem(context.fileSystem));

        auto scene = importer.ReadFile(absolute.string(), decodeFlags(json));
        if (scene == nullptr) {
            return nullptr;
        }
        auto textures = loadTextures(scene, context);
        auto materials = loadMaterials(scene, textures);
        auto model = loadModel(name, scene);

        return std::make_shared<AssimpScene>(name, std::move(textures), std::move(materials), model);
    }
} // namespace neon
