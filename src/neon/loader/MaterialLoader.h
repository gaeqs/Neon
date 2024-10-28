//
// Created by gaeqs on 25/10/2024.
//

#ifndef MATERIALASSETLOADER_H
#define MATERIALASSETLOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/shader/Material.h>

namespace neon {
    class MaterialLoader : public AssetLoader<Material> {
    public:
        MaterialLoader() = default;

        ~MaterialLoader() override = default;

        std::shared_ptr<Material> loadAsset(
            nlohmann::json json,
            AssetLoaderContext context
        ) override;
    };
}


#endif //MATERIALASSETLOADER_H
