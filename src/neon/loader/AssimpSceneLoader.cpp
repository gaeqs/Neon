//
// Created by gaeqs on 4/11/24.
//

#include "AssimpSceneLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <neon/assimp/AssimpNewIOSystem.h>

namespace neon {
    uint32_t AssimpSceneLoader::decodeFlags(const nlohmann::json& json) {
        bool flipUVs = json.value("flip_uvs", false);
        bool flipWindingOrder = json.value("flip_winding_order", false);

        uint32_t flags = aiProcess_Triangulate |
                         aiProcess_JoinIdenticalVertices |
                         aiProcess_SortByPType |
                         aiProcess_RemoveRedundantMaterials |
                         aiProcess_EmbedTextures;
        if (flipUVs) flags |= aiProcess_FlipUVs;
        if (flipWindingOrder) flags |= aiProcess_FlipWindingOrder;
        return flags;
    }

    std::vector<std::shared_ptr<Texture>>
    AssimpSceneLoader::loadTextures(const aiScene* scene,
                                    const std::string& sceneName,
                                    const AssetLoaderContext& context) {
        std::vector<std::shared_ptr<Texture>> textures;
        for (size_t i = 0; i < scene->mNumTextures; ++i) {
            aiTexture* texture = scene->mTextures[i];
            auto name = sceneName + "_" + std::to_string(i);

            TextureCreateInfo info;
            if (texture->mHeight == 0) {
                // Compressed texture
                textures.push_back(Texture::createTextureFromFile(
                    context.application,
                    name,
                    texture->pcData,
                    texture->mWidth,
                    info
                ));
            } else {
                info.image.width = texture->mWidth;
                info.image.height = texture->mHeight;
                info.image.depth = 1;
                info.image.format = TextureFormat::A8R8G8B8;
                textures.push_back(std::make_shared<Texture>(context.application, name, texture->pcData, info));
            }
        }

        return textures;
    }

    std::vector<AssimpMaterial> AssimpSceneLoader::loadMaterials(const aiScene* scene) {
        std::vector<AssimpMaterial> materials;
        for (size_t iMat = 0; iMat < scene->mNumMaterials; ++iMat) {
            auto* aiMat = scene->mMaterials[iMat];
            for(size_t iProp = 0; iProp < aiMat->mNumProperties; ++iProp) {
                auto* aiProp = aiMat->mProperties[iProp];
                Logger::defaultLogger()->debug(MessageBuilder()
                    .print(aiProp->mKey.C_Str())
                    .print(" - ")
                    .print(aiProp->mType));
            }
        }
        return materials;
    }

    std::shared_ptr<AssimpScene>
    AssimpSceneLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) {
        auto file = json["file"];
        if (!file.is_string()) return nullptr;
        std::string fileName = file.get<std::string>();

        Assimp::Importer importer;
        importer.SetIOHandler(new assimp_loader::AssimpNewIOSystem(context.fileSystem));

        auto scene = importer.ReadFile(fileName, decodeFlags(json));
        if (scene == nullptr) return nullptr;
        auto textures = loadTextures(scene, name, context);
        auto materials = loadMaterials(scene);

        return nullptr;
        //auto result = std::make_shared<AssimpScene>(name);
    }
}
