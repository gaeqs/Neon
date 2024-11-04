//
// Created by gaeqs on 4/11/24.
//

#ifndef ASSIMPSCENELOADER_H
#define ASSIMPSCENELOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/assimp/AssimpScene.h>

#include "AssetLoaderHelpers.h"

struct aiScene;

namespace neon {

    class AssimpSceneLoader : public AssetLoader<AssimpScene> {
        static uint32_t decodeFlags(const nlohmann::json& json);

        static std::vector<std::shared_ptr<Texture>>
        loadTextures(const aiScene* scene, const std::string& sceneName, const AssetLoaderContext& context);

        static std::vector<AssimpMaterial>
        loadMaterials(const aiScene* scene);

    public:
        ~AssimpSceneLoader() override = default;

        std::shared_ptr<AssimpScene>
        loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) override;
    };
}


#endif //ASSIMPSCENELOADER_H
