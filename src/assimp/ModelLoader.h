//
// Created by gaelr on 26/10/2022.
//

#ifndef VRTRACKING_MODELLOADER_H
#define VRTRACKING_MODELLOADER_H

#include <vector>
#include <iostream>
#include <memory>

#include <cmrc/cmrc.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/IOSystem.hpp>

#include <engine/collection/ModelCollection.h>
#include <engine/collection/TextureCollection.h>
#include <engine/Room.h>
#include <engine/IdentifiableWrapper.h>
#include <engine/Model.h>
#include <engine/Texture.h>
#include <engine/Material.h>

#include <glm/glm.hpp>

struct ModelLoaderResult {
    bool valid;
    std::vector<Material> materials;
    std::vector<std::pair<IdentifiableWrapper<Model>, uint32_t>> models;
    std::vector<IdentifiableWrapper<Texture>> textures;
};

class ModelLoader {

    ModelCollection& _models;
    TextureCollection& _textures;

    template<typename Vertex>
    IdentifiableWrapper<Model> loadMesh(aiMesh* mesh) const {
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

        return _models.createModel(vertices, indices);
    }

public:

    ModelLoader(ModelCollection& models, TextureCollection& textures) :
            _models(models),
            _textures(textures) {
    }

    explicit ModelLoader(Room* room) :
            _models(room->getModels()),
            _textures(room->getTextures()) {
    }

    explicit ModelLoader(std::shared_ptr<Room> room) :
            _models(room->getModels()),
            _textures(room->getTextures()) {
    }

    template<typename Vertex>
    ModelLoaderResult loadModel(const cmrc::file& file) const {
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
                aiProcess_RemoveRedundantMaterials
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
                aiProcess_RemoveRedundantMaterials
        );
        return loadModel<Vertex>(scene);
    }

    template<typename Vertex>
    ModelLoaderResult loadModel(const aiScene* scene) const {
        if (!scene) return {false};

        ModelLoaderResult result = {true};

        for (int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
            auto* mesh = scene->mMeshes[meshIndex];
            result.models.emplace_back(
                    loadMesh<Vertex>(mesh), mesh->mMaterialIndex);
        }

        std::cout << "---------- MATERIAL -----------" << std::endl;
        for (int i = 0; i < scene->mNumMaterials; ++i) {
            auto* material = scene->mMaterials[i];
            std::cout << "Material: " << i << "\t| Properties: "
                      << material->mNumProperties
                      << std::endl;
            std::cout << material->GetTextureCount(aiTextureType_DIFFUSE)
                      << std::endl;
            std::cout << material->GetTextureCount(aiTextureType_AMBIENT)
                      << std::endl;
            std::cout << material->GetTextureCount(aiTextureType_SPECULAR)
                      << std::endl;
            std::cout << material->GetTextureCount(aiTextureType_BASE_COLOR)
                      << std::endl;
            for (int p = 0; p < material->mNumProperties; ++p) {
                auto* property = material->mProperties[p];
                std::string data = std::string(
                        property->mSemantic, property->mDataLength);
                std::cout << property->mKey.C_Str() << " = "
                          << data << std::endl;
            }
        }
        std::cout << "-------------------------------" << std::endl;


        return result;
    }
};


#endif //VRTRACKING_MODELLOADER_H
