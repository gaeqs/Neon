//
// Created by gaeqs on 3/11/24.
//

#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/model/Mesh.h>

namespace neon
{
    class MeshLoader : public AssetLoader<Mesh>
    {
        static std::unordered_set<std::shared_ptr<Material>> loadMaterials(nlohmann::json& json,
                                                                           const AssetLoaderContext& context);

        static std::vector<float> loadVerticesData(nlohmann::json& json);

        static std::vector<uint32_t> loadIndices(nlohmann::json& json);

      public:
        ~MeshLoader() override = default;

        std::shared_ptr<Mesh> loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) override;
    };
} // namespace neon

#endif //MESHLOADER_H
