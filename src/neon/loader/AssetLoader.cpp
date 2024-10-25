//
// Created by gaeqs on 25/10/24.
//

#include "AssetLoader.h"

#include <nlohmann/json.hpp>

namespace neon {
    AssetLoader::AssetLoader(Application* application, FileSystem* fileSystem, AssetCollection* assetCollection)
        : _application(application), _fileSystem(fileSystem), _assetCollection(assetCollection) {}

    void AssetLoader::loadAsset(std::filesystem::path path) {
        if (_fileSystem == nullptr) return;

        auto file = _fileSystem->readFile(path);
        if (!file.has_value()) return;

        std::stringstream ss()

        nlohmann::json json = nlohmann::json::parse(
10
        );
    }
}
