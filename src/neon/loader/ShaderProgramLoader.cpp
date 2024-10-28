//
// Created by gaeqs on 25/10/2024.
//

#include "ShaderProgramLoader.h"

namespace neon {
    std::shared_ptr<ShaderProgram> ShaderProgramLoader::loadAsset(nlohmann::json json, AssetLoaderContext context) {
        nlohmann::json name = json["name"];
        if (!name.is_string()) return nullptr;

        std::shared_ptr<ShaderProgram> shader = std::make_shared<ShaderProgram>(context.application, name);

        return nullptr;
    }
}
