//
// Created by gaeqs on 25/10/2024.
//

#ifndef SHADERASSETLOADER_H
#define SHADERASSETLOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/shader/ShaderProgram.h>

namespace neon {
    class ShaderProgramLoader : public AssetLoader<ShaderProgram> {
    public:
        ShaderProgramLoader() = default;

        ~ShaderProgramLoader() override = default;

        std::shared_ptr<ShaderProgram> loadAsset(
            std::string name,
            nlohmann::json json,
            AssetLoaderContext context) override;
    };
}


#endif //SHADERASSETLOADER_H
