//
// Created by gaeqs on 1/11/24.
//

#ifndef RENDERPASSSTRATEGYLOADER_H
#define RENDERPASSSTRATEGYLOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/RenderPassStrategy.h>

namespace neon
{
    class RenderPassStrategyLoader : public AssetLoader<RenderPassStrategy>
    {
      public:
        ~RenderPassStrategyLoader() override = default;

        std::shared_ptr<RenderPassStrategy> loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) override;
    };
} // namespace neon

#endif //RENDERPASSSTRATEGYLOADER_H
