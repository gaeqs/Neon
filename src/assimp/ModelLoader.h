//
// Created by gaelr on 26/10/2022.
//

#ifndef VRTRACKING_MODELLOADER_H
#define VRTRACKING_MODELLOADER_H

#include <vector>
#include <iostream>
#include <memory>
#include <string>

#include <cmrc/cmrc.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/IOSystem.hpp>

#include <engine/collection/IdentifiableCollection.h>
#include <engine/collection/TextureCollection.h>
#include <engine/Room.h>
#include <engine/IdentifiableWrapper.h>
#include <engine/model/Model.h>
#include <engine/model/Mesh.h>
#include <engine/Texture.h>
#include "engine/shader/Material.h"

#include <glm/glm.hpp>

struct ModelLoaderResult {
    bool valid;
    IdentifiableWrapper<Model> model;
};

class ModelLoader {

    Application* _application;
    IdentifiableCollection<Model>& _models;
    TextureCollection& _textures;

    template<typename Vertex>
    std::unique_ptr<Mesh> loadMesh(aiMesh* mesh) const {
        std::vector<Vertex> vertices;
        vertices.resize(mesh->mNumVertices);

        for (int i = 0; i < mesh->mNumVertices; ++i) {
            auto aP = mesh->mVertices[i];
            auto aN = mesh->mNormals[i];
            auto aC = mesh->HasVertexColors(0)
                      ? mesh->mColors[0][i] : aiColor4D(0.0, 0.0, 0.0, 0.0);
            auto aT = mesh->mTextureCoords[0][i];

            vertices[i] = Vertex::fromAssimp(
                    glm::vec3(aP.x, aP.y, aP.z),
                    glm::vec3(aN.x, aN.y, aN.z),
                    glm::vec4(aC.r, aC.g, aC.b, aC.a),
                    glm::vec2(aT.x, aT.y)
            );
        }

        std::vector<uint32_t> indices;
        indices.reserve(mesh->mNumFaces * 3);

        for (int i = 0; i < mesh->mNumFaces; ++i) {
            auto face = mesh->mFaces[i];
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }


        auto result = std::make_unique<Mesh>(_application);
        result->uploadVertexData(vertices, indices);
        return std::move(result);
    }

    void loadMaterial(
            std::vector<Material>& vector,
            const std::map<std::string, IdentifiableWrapper<Texture>>& textures,
            const aiMaterial* material) const;

    IdentifiableWrapper<Texture> loadTexture(
            const aiTexture* texture,
            const std::map<aiTexture*,
                    IdentifiableWrapper<Texture>>& loadedTextures) const;

public:

    ModelLoader(
            Application* _application,
            IdentifiableCollection<Model>& models,
            TextureCollection& textures);

    explicit ModelLoader(Room* room);

    explicit ModelLoader(const std::shared_ptr<Room>& room);

    template<typename Vertex>
    [[nodiscard]] ModelLoaderResult loadModel(const cmrc::file& file) const {
        return loadModel<Vertex>(file.begin(), file.size());
    }

    template<typename Vertex>
    ModelLoaderResult loadModel(const void* buffer, size_t length) const {
        Assimp::Importer importer;
        auto scene = importer.ReadFileFromMemory(
                buffer,
                length,
                aiProcess_CalcTangentSpace |
                aiProcess_Triangulate |
                aiProcess_JoinIdenticalVertices |
                aiProcess_GenNormals |
                aiProcess_SortByPType |
                aiProcess_RemoveRedundantMaterials |
                aiProcess_EmbedTextures |
                aiProcess_FlipUVs
        );
        return loadModel<Vertex>(scene);
    }

    template<typename Vertex>
    ModelLoaderResult loadModel(const std::string& directory,
                                const std::string& fileName) const {
        Assimp::Importer importer;
        importer.GetIOHandler()->ChangeDirectory(directory);

        auto scene = importer.ReadFile(
                fileName,
                aiProcess_CalcTangentSpace |
                aiProcess_Triangulate |
                aiProcess_JoinIdenticalVertices |
                aiProcess_GenNormals |
                aiProcess_SortByPType |
                aiProcess_RemoveRedundantMaterials |
                aiProcess_EmbedTextures |
                aiProcess_FlipUVs
        );
        return loadModel<Vertex>(scene);
    }

    template<typename Vertex>
    ModelLoaderResult loadModel(const aiScene* scene) const {
        if (!scene) return {false};

        ModelLoaderResult result = {true};

        std::map<std::string, IdentifiableWrapper<Texture>> textures;
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<Material> materials;
        meshes.reserve(scene->mNumMeshes);
        materials.reserve(scene->mNumMaterials);

        std::map<aiTexture*, IdentifiableWrapper<Texture>> loadedTextures;
        for (int i = 0; i < scene->mNumTextures; ++i) {
            auto* aiTexture = scene->mTextures[i];
            auto name = "*" + std::to_string(i);
            auto texture = loadTexture(aiTexture, loadedTextures);
            textures.emplace(name, texture);
            loadedTextures.emplace(aiTexture, texture);
        }

        for (int i = 0; i < scene->mNumMaterials; ++i) {
            auto* material = scene->mMaterials[i];
            loadMaterial(materials, textures, material);
        }

        for (int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
            auto* mesh = scene->mMeshes[meshIndex];
            auto meshResult = loadMesh<Vertex>(mesh);
            meshResult->setMaterial(materials[mesh->mMaterialIndex]);
            meshes.emplace_back(std::move(meshResult));
        }

        result.model = _models.create(meshes);

        return result;
    }
};


#endif //VRTRACKING_MODELLOADER_H
