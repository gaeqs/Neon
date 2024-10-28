//
// Created by gaeqs on 25/10/24.
//

#include "AssetLoader.h"

#include <nlohmann/json.hpp>
#include <utility>

#include "AssetLoaderCollection.h"

namespace neon {
    AssetLoaderContext::AssetLoaderContext(
        Application* app,
        FileSystem* fs,
        AssetLoaderCollection* lc,
        AssetCollection* c) : application(app),
                              fileSystem(fs),
                              loaders(lc),
                              collection(c) {}
}
