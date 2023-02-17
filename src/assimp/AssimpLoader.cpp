//
// Created by gaeqs on 14/02/2023.
//

#include "AssimpLoader.h"

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <assimp/AssimpMaterialParameters.h>
#include <assimp/AssimpGeometry.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/IOSystem.hpp>

#include <engine/shader/Material.h>
#include <engine/render/Texture.h>
#include <engine/structure/Room.h>
#include <engine/model/Mesh.h>

namespace neon::assimp_loader {

    using Tex = IdentifiableWrapper<Texture>;
    using Mat = IdentifiableWrapper<Material>;

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
                        const std::map<aiTexture*, Tex>& loadedTextures,
                        const LoaderInfo& info) {

            // Let's check if the texture is already loaded!
            for (const auto& [aiTexture, actualTexture]: loadedTextures) {
                if (aiTexture->mWidth == texture->mWidth
                    && aiTexture->mHeight == texture->mHeight) {
                    return actualTexture;
                }
            }

            if (texture->mHeight == 0) {
                // Compressed format! Let's stbi decide.
                return info.room->getTextures().createTextureFromFile(
                        texture->pcData,
                        texture->mWidth
                );
            }

            TextureCreateInfo createInfo;
            createInfo.image.width = texture->mWidth;
            createInfo.image.height = texture->mHeight;
            createInfo.image.depth = 1;
            createInfo.image.format = TextureFormat::A8R8G8B8;

            // Value is always ARGB8888
            return info.room->getTextures().create(texture->pcData, createInfo);
        }

        void loadTextures(
                const aiScene* scene,
                std::map<std::string, Tex>& textures,
                std::map<aiTexture*, Tex>& loadedTextures,
                const LoaderInfo& info) {

            for (int i = 0; i < scene->mNumTextures; ++i) {
                auto* aiTexture = scene->mTextures[i];
                auto name = "*" + std::to_string(i);
                auto texture = loadTexture(aiTexture, loadedTextures, info);
                textures.emplace(name, texture);
                loadedTextures.emplace(aiTexture, texture);
            }
        }

        Mat loadMaterial(const aiMaterial* material,
                         const std::map<std::string, Tex>& textures,
                         const LoaderInfo& info) {
            static const auto getTextureId = [](const aiString& string) {
                return std::string(string.data, std::min(string.length, 2u));
            };

            auto mInfo = info.materialCreateInfo;
            mInfo.descriptions.vertex = info.vertexParser.description;
            mInfo.descriptions.instance = info.instanceData.description;

            auto m = info.room->getMaterials().create(mInfo);

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
            for (int i = 0; i < scene->mNumMaterials; ++i) {
                auto* material = scene->mMaterials[i];
                materials.push_back(loadMaterial(material, textures, info));
            }
        }

        std::unique_ptr<Mesh> loadMesh(const aiMesh* mesh,
                                       Mat material,
                                       const LoaderInfo& info) {
            std::vector<char> dataArray;
            dataArray.reserve(mesh->mNumVertices
                              * info.vertexParser.structSize);

            auto tangents = assimp_geometry::calculateTangents(mesh);

            for (int i = 0; i < mesh->mNumVertices; ++i) {
                auto aP = mesh->mVertices[i];
                auto aN = mesh->mNormals[i];
                auto aC = mesh->HasVertexColors(0)
                          ? mesh->mColors[0][i] : aiColor4D(0.0, 0.0, 0.0, 0.0);
                auto aT = mesh->mTextureCoords[0][i];


                if (info.flipNormals) {
                    aN = -aN;
                }

                VertexParserData parserData{
                        glm::vec3(aP.x, aP.y, aP.z),
                        glm::vec3(aN.x, aN.y, aN.z),
                        tangents[i],
                        glm::vec4(aC.r, aC.g, aC.b, aC.a),
                        glm::vec2(aT.x, aT.y)
                };

                info.vertexParser.parseFunction(parserData, dataArray);
            }


            std::vector<uint32_t> indices;
            indices.reserve(mesh->mNumFaces * 3);

            for (int i = 0; i < mesh->mNumFaces; ++i) {
                auto face = mesh->mFaces[i];
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }

            auto result = std::make_unique<Mesh>(info.room, material);
            result->reinitializeVertexData(dataArray, indices);

            return result;
        }

        void loadMeshes(const aiScene* scene,
                        std::vector<std::unique_ptr<Mesh>>& meshes,
                        const std::vector<Mat>& materials,
                        const LoaderInfo& info) {
            for (int i = 0; i < scene->mNumMeshes; ++i) {
                auto* aiMesh = scene->mMeshes[i];
                meshes.push_back(loadMesh(
                        aiMesh,
                        materials[aiMesh->mMaterialIndex],
                        info
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
        importer.GetIOHandler()->ChangeDirectory(directory);
        auto scene = importer.ReadFile(file, decodeFlags(info));
        std::filesystem::current_path(previous);

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
        if (!scene) return {false};

        // Init collections
        std::map<std::string, Tex> textures;
        std::map<aiTexture*, Tex> loadedTextures;

        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<Mat> materials;
        meshes.reserve(scene->mNumMeshes);
        materials.reserve(scene->mNumMaterials);

        loadTextures(scene, textures, loadedTextures, info);
        loadMaterials(scene, materials, textures, info);
        loadMeshes(scene, meshes, materials, info);

        auto model = info.room->getModels().create(meshes);
        model->defineInstanceStruct(
                info.instanceData.type,
                info.instanceData.size
        );

        return {true, model};
    }
}