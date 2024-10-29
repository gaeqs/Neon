//
// Created by gaeqs on 25/10/24.
//

#include "AssetLoader.h"

#include <neon/structure/Application.h>

#include <nlohmann/json.hpp>

namespace neon {
    AssetLoaderContext::AssetLoaderContext(
        Application* app,
        std::filesystem::path* p,
        FileSystem* fs,
        AssetLoaderCollection* lc,
        AssetCollection* c)
        : application(app),
          path(p == nullptr ? std::optional<std::filesystem::path>() : *p),
          fileSystem(fs),
          loaders(lc == nullptr ? &app->getAssetLoaders() : lc),
          collection(c == nullptr ? &app->getAssets() : c) {}
}
