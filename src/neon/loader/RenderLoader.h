//
// Created by gaeqs on 1/11/24.
//

#ifndef RENDERLOADER_H
#define RENDERLOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/Render.h>

namespace neon
{
    class RenderLoader : public AssetLoader<Render>
    {
      public:
        ~RenderLoader() override = default;

        std::shared_ptr<Render> loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) override;
    };
} // namespace neon

#endif //RENDERLOADER_H
