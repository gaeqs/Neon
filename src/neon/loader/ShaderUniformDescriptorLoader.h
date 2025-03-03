//
// Created by gaeqs on 1/11/24.
//

#ifndef SHADERUNIFORMDESCRIPTORLOADER_H
#define SHADERUNIFORMDESCRIPTORLOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/shader/ShaderUniformDescriptor.h>

namespace neon {
    class ShaderUniformDescriptorLoader : public AssetLoader<ShaderUniformDescriptor> {
    public:
        ~ShaderUniformDescriptorLoader() override = default;

        std::shared_ptr<ShaderUniformDescriptor>
        loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) override;
    };
}


#endif //SHADERUNIFORMDESCRIPTORLOADER_H
