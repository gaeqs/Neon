//
// Created by gaeqs on 25/10/24.
//

#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <memory>
#include <utility>
#include <optional>
#include <nlohmann/json.hpp>

namespace neon
{
    class Application;
    class FileSystem;
    class AssetCollection;
    class AssetLoaderCollection;
    class CommandBuffer;

    class AbstractAssetLoader
    {
      public:
        virtual ~AbstractAssetLoader() = default;
    };

    struct AssetLoaderContext
    {
        Application* application;
        std::optional<std::filesystem::path> path;
        FileSystem* fileSystem;
        AssetLoaderCollection* loaders;
        AssetCollection* collection;
        CommandBuffer* commandBuffer;
        AssetCollection* localCollection;

        AssetLoaderContext(Application* app, std::filesystem::path* path = nullptr, FileSystem* fileSystem = nullptr,
                           AssetLoaderCollection* loaders = nullptr, AssetCollection* collection = nullptr,
                           CommandBuffer* commandBuffer = nullptr, AssetCollection* localCollection = nullptr);
    };

    template<typename AssetType>
    class AssetLoader : public AbstractAssetLoader
    {
      public:
        ~AssetLoader() override = default;

        virtual std::shared_ptr<AssetType> loadAsset(std::string name, nlohmann::json json,
                                                     AssetLoaderContext context) = 0;
    };

    template<typename Type, typename Loader>
    concept WellStructuredLoader = requires(Loader l) {
        {
            l.loadAsset(std::declval<std::string>(), std::declval<nlohmann::json>(), std::declval<AssetLoaderContext>())
        } -> std::convertible_to<std::shared_ptr<Type>>;
        requires std::derived_from<Loader, AssetLoader<Type>>;
    };
} // namespace neon

#endif //RESOURCELOADER_H
