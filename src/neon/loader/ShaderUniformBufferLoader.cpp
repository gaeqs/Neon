//
// Created by gaeqs on 21/04/2025.
//

#include "ShaderUniformBufferLoader.h"

#include "AssetLoaderHelpers.h"

namespace neon
{
    std::shared_ptr<ShaderUniformBuffer> ShaderUniformBufferLoader::loadAsset(std::string name, nlohmann::json json,
                                                                              AssetLoaderContext context)
    {
        auto descriptor = getAsset<ShaderUniformDescriptor>(json["descriptor"], context);
        if (descriptor == nullptr) {
            return nullptr;
        }

        return std::make_shared<ShaderUniformBuffer>(name, descriptor);
    }
} // namespace neon