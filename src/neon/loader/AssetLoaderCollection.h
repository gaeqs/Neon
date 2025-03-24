//
// Created by gaeqs on 25/10/2024.
//

#ifndef ASSETLOADERCOLLECTION_H
#define ASSETLOADERCOLLECTION_H

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <neon/loader/AssetLoader.h>

namespace neon
{
    class AssetLoaderCollection
    {
        std::unordered_map<std::type_index, std::unique_ptr<AbstractAssetLoader>> _loaders;

      public:
        explicit AssetLoaderCollection(bool loadDefault);

        AssetLoaderCollection(const AssetLoaderCollection& other) = delete;

        template<typename Type, typename Loader>
            requires WellStructuredLoader<Type, Loader>
        void registerLoader()
        {
            _loaders[std::type_index(typeid(Type))] = std::make_unique<Loader>();
        }

        template<typename Type>
        void registerLoader(std::unique_ptr<AssetLoader<Type>> loader)
        {
            if (loader == nullptr) {
                return;
            }
            _loaders[std::type_index(typeid(Type))] = std::move(loader);
        }

        template<typename Type>
        void removeLoader()
        {
            _loaders.erase(typeid(Type));
        }

        template<typename Type>
        std::optional<AssetLoader<Type>*> getLoaderFor() const
        {
            auto it = _loaders.find(typeid(Type));
            if (it == _loaders.end()) {
                return {};
            }
            return {static_cast<AssetLoader<Type>*>(it->second.get())};
        }
    };
} // namespace neon

#endif //ASSETLOADERCOLLECTION_H
