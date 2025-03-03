//
// Created by gaeqs on 5/11/24.
//

#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <neon/assimp/AssimpScene.h>
#include <neon/loader/AssetLoader.h>
#include <neon/render/model/Mesh.h>
#include <neon/render/model/Model.h>

namespace neon {
    class ModelLoader : public AssetLoader<Model> {

        static size_t getEntrySizeInFloats(LocalVertexEntry entry);

        static std::shared_ptr<Material> loadMaterial(nlohmann::json& metadata,
                                                      const AssimpMaterial& assimpMaterial,
                                                      const AssetLoaderContext& context);

        static std::shared_ptr<Mesh> loadMesh(nlohmann::json& metadata,
                                              const LocalMesh& localMesh,
                                              const std::vector<std::shared_ptr<Material>>& materials,
                                              const AssetLoaderContext& context);

        static void applyAssimpModel(nlohmann::json& metadata,
                                     const std::shared_ptr<AssimpScene>& scene,
                                     ModelCreateInfo& info,
                                     const AssetLoaderContext& context);

    public:
        ~ModelLoader() override = default;

        std::shared_ptr<Model>
        loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) override;
    };
}


#endif //MODELLOADER_H
