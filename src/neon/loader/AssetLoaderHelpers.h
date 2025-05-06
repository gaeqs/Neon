//
// Created by gaeqs on 29/10/24.
//

#ifndef ASSETLOADERHELPERS_H
#define ASSETLOADERHELPERS_H

#include <memory>
#include <neon/structure/Application.h>
#include <neon/structure/collection/AssetCollection.h>
#include <neon/filesystem/FileSystem.h>
#include <neon/logging/Logger.h>

namespace neon
{
    template<typename T>
    struct AssetGeneralProperties
    {
        std::string name;
        std::shared_ptr<T> present = nullptr;
        bool save = false;
        bool override = false;
        bool saveWeak = false;
        std::optional<std::string> error = {};

        static AssetGeneralProperties<T> sendError(std::string error)
        {
            AssetGeneralProperties<T> prop;
            prop.error = {error};
            return prop;
        }
    };

    template<typename T>
    AssetGeneralProperties<T> fetchGeneralProperties(const nlohmann::json& json, const AssetLoaderContext& context)
    {
        if (!json.contains("name")) {
            return AssetGeneralProperties<T>::sendError("Name not found!");
        }
        AssetGeneralProperties<T> prop;
        prop.name = json["name"];

        if (context.collection != nullptr && json.value("use_present", false)) {
            if (auto result = context.collection->get<T>(prop.name); result.has_value()) {
                prop.present = std::move(result.value());
            }
        }

        prop.save = json.value("save", prop.save);
        prop.override = json.value("override", prop.override);
        prop.saveWeak = json.value("save_weak", prop.saveWeak);

        return prop;
    }

    template<typename T>
    void applyGeneralProperties(std::shared_ptr<T> asset, const AssetGeneralProperties<T>& prop,
                                const AssetLoaderContext& context)
    {
        if (prop.save && context.collection != nullptr) {
            if (prop.override || !context.collection->get(asset->getIdentifier()).has_value()) {
                context.collection->store(asset, prop.saveWeak ? AssetStorageMode::WEAK : AssetStorageMode::PERMANENT);
            }
        }
    }

    template<typename T>
    std::shared_ptr<T> findAssetInCollection(const std::string& name, const AssetLoaderContext& context)
    {
        if (context.localCollection != nullptr) {
            if (auto result = context.localCollection->get<T>(name); result.has_value()) {
                return result.value();
            }
        }
        if (context.collection == nullptr) {
            return nullptr;
        }
        auto optional = context.collection->get<T>(name);
        if (optional.has_value()) {
            return optional.value();
        }
        return nullptr;
    }

    inline std::optional<File> getFile(const std::filesystem::path& path, AssetLoaderContext context)
    {
        if (context.fileSystem == nullptr || context.loaders == nullptr) {
            return {};
        }
        context.path = context.path.has_value() ? context.path.value().parent_path() / path : path;
        return context.fileSystem->readFile(context.path.value());
    }

    template<typename T>
    std::shared_ptr<T> loadAssetFromFile(const std::filesystem::path& path, AssetLoaderContext context)
    {
        if (context.fileSystem == nullptr || context.loaders == nullptr) {
            return nullptr;
        }

        auto loader = context.loaders->getLoaderFor<T>();
        if (!loader.has_value()) {
            return nullptr;
        }

        context.path = context.path.has_value() ? context.path.value().parent_path() / path : path;
        auto file = context.fileSystem->readFile(context.path.value());
        if (!file.has_value()) {
            return nullptr;
        }
        auto json = file.value().toJson();
        if (!json.has_value()) {
            return nullptr;
        }

        AssetGeneralProperties<T> prop = fetchGeneralProperties<T>(json.value(), context);
        if (prop.error.has_value()) {
            logger.error(prop.error.value());
            return nullptr;
        }

        if (prop.present) {
            return prop.present;
        }

        auto result = loader.value()->loadAsset(prop.name, json.value(), context);
        if (result != nullptr) {
            applyGeneralProperties(result, prop, context);
        }
        return result;
    }

    template<typename T>
    std::shared_ptr<T> loadAssetFromData(const nlohmann::json& json, const AssetLoaderContext& context)
    {
        if (context.loaders == nullptr) {
            return nullptr;
        }

        auto loader = context.loaders->getLoaderFor<T>();
        if (!loader.has_value()) {
            return nullptr;
        }

        AssetGeneralProperties<T> prop = fetchGeneralProperties<T>(json, context);
        if (prop.error.has_value()) {
            logger.error(prop.error.value());
            return nullptr;
        }

        if (prop.present) {
            return prop.present;
        }

        auto result = loader.value()->loadAsset(prop.name, json, context);
        if (result != nullptr) {
            applyGeneralProperties(result, prop, context);
        }
        return result;
    }

    template<typename T>
    std::shared_ptr<T> getAsset(const nlohmann::json& json, const AssetLoaderContext& context)
    {
        // TYPES
        // "myElement": "FILE"
        // "myElement": "A:ASSET"
        // "myElement": {...} // Raw load.
        // "myElement": ["FILE", "A:ASSET", ..., {...}]
        // The algorithm will try to fetch the asset from the given sources in order!

        if (json.is_null()) {
            return nullptr;
        }

        std::vector<nlohmann::json> elements;
        if (json.is_array()) {
            elements = json;
        } else {
            elements.push_back(json);
        }

        for (auto& element : elements) {
            if (element.is_string()) {
                auto string = element.get<std::string>();
                if (string.starts_with("A:") || string.starts_with("a:")) {
                    // Find asset.
                    auto asset = findAssetInCollection<T>(string.substr(2), context);
                    if (asset != nullptr) {
                        return asset;
                    }
                } else {
                    // Find file
                    auto asset = loadAssetFromFile<T>(std::filesystem::path(string), context);
                    if (asset != nullptr) {
                        return asset;
                    }
                }
            }

            if (element.is_object()) {
                // Load Raw
                auto asset = loadAssetFromData<T>(element, context);
                if (asset != nullptr) {
                    return asset;
                }
            }
        }

        return nullptr;
    }
} // namespace neon

#endif //ASSETLOADERHELPERS_H
