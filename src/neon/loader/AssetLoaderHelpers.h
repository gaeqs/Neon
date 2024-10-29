//
// Created by gaeqs on 29/10/24.
//

#ifndef ASSETLOADERHELPERS_H
#define ASSETLOADERHELPERS_H

#include <memory>

namespace neon {
    template<typename T>
    std::shared_ptr<T> findAssetInCollection(const std::string& name, const AssetLoaderContext& context) {
        if (context.collection == nullptr) return nullptr;
        auto optional = context.collection->template get<T>(name);
        if(optional.has_value()) return optional.value();
        return nullptr;
    }

    template<typename T>
    std::shared_ptr<T> loadAssetFromFile(const std::filesystem::path& path, AssetLoaderContext context) {
        if (context.fileSystem == nullptr || context.loaders == nullptr) return nullptr;

        auto loader = context.loaders->getLoaderFor<T>();
        if (!loader.has_value()) return nullptr;

        context.path = context.path.has_value() ? context.path.value() / path : path;
        auto file = context.fileSystem->readFile(context.path.value());
        if (!file.has_value()) return nullptr;
        auto json = file.value().toJson();
        if (!json.has_value()) return nullptr;

        return loader.value()->loadAsset(json.value(), context);
    }

    template<typename T>
    std::shared_ptr<T> loadAssetFromData(const nlohmann::json& json, const AssetLoaderContext& context) {
        if (context.loaders == nullptr) return nullptr;

        auto loader = context.loaders->getLoaderFor<T>();
        if (!loader.has_value()) return nullptr;

        return loader.value()->loadAsset(json, context);
    }

    template<typename T>
    std::shared_ptr<T> getAsset(const nlohmann::json& json, const AssetLoaderContext& context) {
        // TYPES
        // "myElement": "FILE"
        // "myElement": "A:ASSET"
        // "myElement": {...} // Raw load.
        // "myElement": ["FILE", "A:ASSET", ..., {...}]
        // The algorithm will try to fetch the asset from the given sources in order!

        std::vector<nlohmann::json> elements;
        if (json.is_array()) {
            elements = json;
        } else {
            elements.push_back(json);
        }

        for (auto& element: elements) {
            if (element.is_string()) {
                auto string = element.get<std::string>();
                if (string.starts_with("A:") || string.starts_with("a:")) {
                    // Find asset.
                    auto asset = findAssetInCollection<T>(string.substr(2), context);
                    if (asset != nullptr) return asset;
                } else {
                    // Find file
                    auto asset = loadAssetFromFile<T>(std::filesystem::path(string), context);
                    if (asset != nullptr) return asset;
                }
            }

            if (element.is_object()) {
                // Load Raw
                auto asset = loadAssetFromData<T>(element, context);
                if (asset != nullptr) return asset;
            }
        }

        return nullptr;
    }
}


#endif //ASSETLOADERHELPERS_H
