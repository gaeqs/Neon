//
// Created by gaeqs on 14/02/2023.
//

#include "AssimpLoader.h"
#include "neon/structure/collection/AssetCollection.h"

#include <memory>
#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <neon/assimp/AssimpMaterialParameters.h>
#include <neon/assimp/AssimpGeometry.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/IOSystem.hpp>

#include <neon/render/shader/Material.h>
#include <neon/render/texture/Texture.h>
#include <neon/structure/Room.h>
#include <neon/render/model/Mesh.h>
#include <neon/render/model/Model.h>

#include "AssimpNewIOSystem.h"

namespace neon::assimp_loader {
    using Tex = std::shared_ptr<Texture>;
    using Mat = std::shared_ptr<Material>;

    // Private methods
    namespace {
        uint32_t decodeFlags(const LoaderInfo& info) {
            uint32_t flags = aiProcess_Triangulate |
                             aiProcess_JoinIdenticalVertices |
                             aiProcess_SortByPType |
                             aiProcess_RemoveRedundantMaterials |
                             aiProcess_EmbedTextures;
            if (info.flipUVs) flags |= aiProcess_FlipUVs;
            if (info.flipWindingOrder) flags |= aiProcess_FlipWindingOrder;
            return flags;
        }

        Tex loadTexture(const aiTexture* texture,
                        const std::string& name,
                        const std::map<aiTexture*, Tex>& loadedTextures,
                        const LoaderInfo& info) {
            // Let's check if the texture is already loaded!
            for (const auto& [aiTexture, actualTexture]: loadedTextures) {
                if (aiTexture->mWidth == texture->mWidth
                    && aiTexture->mHeight == texture->mHeight) {
                    return actualTexture;
                }
            }

            TextureCreateInfo createInfo;
            createInfo.commandBuffer = info.commandBuffer;

            if (texture->mHeight == 0) {
                // Compressed format! Let's stbi decide.
                return Texture::createTextureFromFile(
                    info.application,
                    texture->mFilename.C_Str(),
                    texture->pcData,
                    texture->mWidth,
                    createInfo
                );
            }

            createInfo.image.width = texture->mWidth;
            createInfo.image.height = texture->mHeight;
            createInfo.image.depth = 1;
            createInfo.image.format = TextureFormat::A8R8G8B8;

            // Value is always ARGB8888
            return std::make_shared<Texture>(
                info.application,
                name,
                texture->pcData,
                createInfo
            );
        }

        void loadTextures(
            const aiScene* scene,
            std::map<std::string, Tex>& textures,
            std::map<aiTexture*, Tex>& loadedTextures,
            const LoaderInfo& info) {
            for (int i = 0; i < scene->mNumTextures; ++i) {
                auto* aiTexture = scene->mTextures[i];
                auto name = "*" + std::to_string(i);
                auto texture = loadTexture(aiTexture, name,
                                           loadedTextures, info);
                textures.emplace(name, texture);
                loadedTextures.emplace(aiTexture, texture);
            }
        }

        Mat loadMaterial(
            uint32_t index,
            const aiMaterial* material,
            const std::map<std::string, Tex>& textures,
            const LoaderInfo& info) {
            static const auto getTextureId = [](const aiString& string) {
                return std::string(string.data, std::min(string.length, 2u));
            };

            auto mInfo = info.materialCreateInfo;
            mInfo.descriptions.vertex.push_back(info.vertexParser.description);

            for (auto iData: info.instanceDatas) {
                mInfo.descriptions.instance.push_back(iData.description);
            }

            auto m = std::make_shared<Material>(
                info.application,
                info.name + "_material_" + std::to_string(index),
                mInfo
            );

            aiColor3D color;
            int b;
            float f;
            aiString t;

            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) ==
                aiReturn_SUCCESS) {
                m->pushConstant(DIFFUSE_COLOR, color);
            }
            if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) ==
                aiReturn_SUCCESS) {
                m->pushConstant(SPECULAR_COLOR, color);
            }
            if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) ==
                aiReturn_SUCCESS) {
                m->pushConstant(AMBIENT_COLOR, color);
            }
            if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) ==
                aiReturn_SUCCESS) {
                m->pushConstant(EMISSIVE_COLOR, color);
            }
            if (material->Get(AI_MATKEY_COLOR_TRANSPARENT, color) ==
                aiReturn_SUCCESS) {
                m->pushConstant(TRANSPARENT_COLOR, color);
            }
            if (material->Get(AI_MATKEY_ENABLE_WIREFRAME, b) ==
                aiReturn_SUCCESS) {
                m->pushConstant(WIREFRAME, b);
            }
            if (material->Get(AI_MATKEY_TWOSIDED, b) == aiReturn_SUCCESS) {
                m->pushConstant(TWO_SIDED, b);
            }
            if (material->Get(AI_MATKEY_OPACITY, f) == aiReturn_SUCCESS) {
                m->pushConstant(OPACITY, b);
            }
            if (material->Get(AI_MATKEY_SHININESS, f) == aiReturn_SUCCESS) {
                m->pushConstant(SHININESS, b);
            }
            if (material->Get(AI_MATKEY_SHININESS_STRENGTH, f) ==
                aiReturn_SUCCESS) {
                m->pushConstant(SHININESS_STRENGTH, b);
            }
            if (material->Get(AI_MATKEY_REFRACTI, f) == aiReturn_SUCCESS) {
                m->pushConstant(REFRACT_INDEX, b);
            }
            if (material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), t) ==
                aiReturn_SUCCESS) {
                auto texture = textures.find(
                    std::string(t.data, std::min(t.length, 2u)));
                if (texture != textures.end()) {
                    m->setTexture(DIFFUSE_TEXTURE, texture->second);
                }
            }
            if (material->Get(AI_MATKEY_TEXTURE_SPECULAR(0), t) ==
                aiReturn_SUCCESS) {
                auto texture = textures.find(getTextureId(t));
                if (texture != textures.end()) {
                    m->setTexture(SPECULAR_TEXTURE, texture->second);
                }
            }
            if (material->Get(AI_MATKEY_TEXTURE_AMBIENT(0), t) ==
                aiReturn_SUCCESS) {
                auto texture = textures.find(getTextureId(t));
                if (texture != textures.end()) {
                    m->setTexture(AMBIENT_TEXTURE, texture->second);
                }
            }
            if (material->Get(AI_MATKEY_TEXTURE_EMISSIVE(0), t) ==
                aiReturn_SUCCESS) {
                auto texture = textures.find(getTextureId(t));
                if (texture != textures.end()) {
                    m->setTexture(AMBIENT_TEXTURE, texture->second);
                }
            }
            if (material->Get(AI_MATKEY_TEXTURE_DISPLACEMENT(0), t) ==
                aiReturn_SUCCESS) {
                auto texture = textures.find(getTextureId(t));
                if (texture != textures.end()) {
                    m->setTexture(DISPLACEMENT_TEXTURE, texture->second);
                }
            }
            if (material->Get(AI_MATKEY_TEXTURE_NORMALS(0), t) ==
                aiReturn_SUCCESS) {
                auto texture = textures.find(getTextureId(t));
                if (texture != textures.end()) {
                    m->setTexture(NORMAL_TEXTURE, texture->second);
                }
            }

            return m;
        }

        void loadMaterials(const aiScene* scene,
                           std::vector<Mat>& materials,
                           const std::map<std::string, Tex>& textures,
                           const LoaderInfo& info) {
            for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
                auto* material = scene->mMaterials[i];
                materials.push_back(loadMaterial(i, material, textures, info));
            }
        }

        LocalMesh loadLocalMesh(const aiMesh* mesh, bool flipNormals) {
            LocalMesh local;
            local.vertices.reserve(mesh->mNumVertices);
            local.indices.reserve(mesh->mNumFaces * 3);

            auto tangents = assimp_geometry::calculateTangents(mesh);

            for (int i = 0; i < mesh->mNumVertices; ++i) {
                auto aP = mesh->mVertices[i];
                auto aN = mesh->mNormals[i];
                auto aC = mesh->HasVertexColors(0)
                              ? mesh->mColors[0][i]
                              : aiColor4D(0.0, 0.0, 0.0, 0.0);
                auto aT = mesh->mTextureCoords[0][i];


                if (flipNormals) {
                    aN = -aN;
                }

                VertexParserData parserData{
                    rush::Vec3f(aP.x, aP.y, aP.z),
                    rush::Vec3f(aN.x, aN.y, aN.z),
                    tangents[i],
                    rush::Vec4f(aC.r, aC.g, aC.b, aC.a),
                    rush::Vec2f(aT.x, aT.y)
                };

                local.vertices.push_back(parserData);
            }

            for (int i = 0; i < mesh->mNumFaces; ++i) {
                auto face = mesh->mFaces[i];
                local.indices.push_back(face.mIndices[0]);
                local.indices.push_back(face.mIndices[1]);
                local.indices.push_back(face.mIndices[2]);
            }

            return local;
        }

        std::shared_ptr<Mesh>
        loadMesh(const aiMesh* mesh,
                 const Mat& material,
                 const LoaderInfo& info,
                 const std::unique_ptr<LocalModel>& localModel) {
            std::vector<char> dataArray;
            dataArray.reserve(mesh->mNumVertices
                              * info.vertexParser.structSize);

            auto tangents = assimp_geometry::calculateTangents(mesh);

            LocalMesh local = loadLocalMesh(mesh, info.flipNormals);

            for (auto& vertex: local.vertices) {
                info.vertexParser.parseFunction(vertex, dataArray);
            }

            auto result = std::make_shared<Mesh>(
                info.application,
                info.name + "_" + mesh->mName.C_Str(),
                material
            );

            if (info.storeAssets) {
                info.application->getAssets()
                        .store(result, info.assetStorageMode);
            }
            result->uploadVertices(dataArray);
            result->uploadIndices(local.indices);

            if (localModel) {
                localModel->meshes.push_back(std::move(local));
            }

            return result;
        }

        void loadMeshes(const aiScene* scene,
                        std::vector<std::shared_ptr<Mesh>>& meshes,
                        const std::vector<Mat>& materials,
                        const LoaderInfo& info,
                        const std::unique_ptr<LocalModel>& localModel) {
            for (int i = 0; i < scene->mNumMeshes; ++i) {
                auto* aiMesh = scene->mMeshes[i];
                auto mat = materials.size() > aiMesh->mMaterialIndex
                               ? materials[aiMesh->mMaterialIndex]
                               : nullptr;
                meshes.push_back(loadMesh(
                    aiMesh,
                    mat,
                    info,
                    localModel
                ));
            }
        }
    }

    Result load(const cmrc::file& file,
                const LoaderInfo& info) {
        Assimp::Importer importer;
        auto scene = importer.ReadFileFromMemory(
            file.begin(), file.size(), decodeFlags(info));

        return load(scene, info);
    }

    Result load(const std::string& directory,
                const std::string& file,
                const LoaderInfo& info) {
        Assimp::Importer importer;

        auto previous = std::filesystem::current_path();
        importer.SetIOHandler(new AssimpNewIOSystem(directory));
        auto scene = importer.ReadFile(file, decodeFlags(info));
        return load(scene, info);
    }

    Result load(const void* buffer,
                size_t length,
                const LoaderInfo& info) {
        Assimp::Importer importer;
        auto scene = importer.ReadFileFromMemory(
            buffer, length, decodeFlags(info));

        return load(scene, info);
    }

    Result load(const aiScene* scene,
                const LoaderInfo& info) {
        if (!scene) return {LoadError::INVALID_SCENE};

        if (!info.loadGPUModel) {
            // Local mode only.
            if (!info.loadLocalModel) return {{}, nullptr, nullptr};
            auto local = std::unique_ptr<LocalModel>();
            for (size_t i = 0; i < scene->mNumMeshes; ++i) {
                auto* mesh = scene->mMeshes[i];
                local->meshes.push_back(loadLocalMesh(mesh, info.flipNormals));
            }
            return {{}, nullptr, std::move(local)};
        }

        // Init collections
        std::map<std::string, Tex> textures;
        std::map<aiTexture*, Tex> loadedTextures;

        ModelCreateInfo modelInfo;
        modelInfo.instanceDataProvider = info.instanceDataProvider;
        modelInfo.meshes.reserve(scene->mNumMeshes);

        for (auto iData: info.instanceDatas) {
            modelInfo.instanceTypes.push_back(iData.type);
            modelInfo.instanceSizes.push_back(iData.size);
        }

        std::vector<Mat> materials;

        loadTextures(scene, textures, loadedTextures, info);
        if (info.loadMaterials) {
            materials.reserve(scene->mNumMaterials);
            loadMaterials(scene, materials, textures, info);
        }

        std::unique_ptr<LocalModel> local = info.loadLocalModel
                                                ? std::make_unique<LocalModel>()
                                                : nullptr;
        loadMeshes(scene, modelInfo.meshes, materials, info, local);


        auto model = std::make_shared<Model>(
            info.application,
            info.name,
            modelInfo
        );

        info.application->getAssets().store(model, AssetStorageMode::WEAK);

        return {{}, model, std::move(local)};
    }
}
