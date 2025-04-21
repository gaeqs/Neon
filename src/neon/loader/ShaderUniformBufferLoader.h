//
// Created by gaeqs on 21/04/2025.
//

#ifndef SHADERUNIFORMBUFFERLOADER_H
#define SHADERUNIFORMBUFFERLOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/shader/ShaderUniformBuffer.h>
#include <neon/render/shader/ShaderUniformDescriptor.h>

namespace neon
{
    class ShaderUniformBufferLoader : public AssetLoader<ShaderUniformBuffer>
    {
      public:
        ~ShaderUniformBufferLoader() override = default;

        std::shared_ptr<ShaderUniformBuffer> loadAsset(std::string name, nlohmann::json json,
                                                       AssetLoaderContext context) override;
    };
} // namespace neon

#endif // SHADERUNIFORMBUFFERLOADER_H
