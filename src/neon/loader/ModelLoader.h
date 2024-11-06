//
// Created by gaeqs on 5/11/24.
//

#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/model/Model.h>

namespace neon {
    class ModelLoader : public AssetLoader<Model> {
    public:
        ~ModelLoader() override = default;

        std::shared_ptr<Model>
        loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) override;
    };
}


#endif //MODELLOADER_H
