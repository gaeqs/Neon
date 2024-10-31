//
// Created by gaeqs on 02/03/2023.
//

#include "AssetCollection.h"

#include <ranges>

namespace neon {

    std::optional<std::shared_ptr<Asset>>
    AssetCollection::get(const AssetIdentifier& identifier) const {
        auto itType = _assets.find(identifier.type);
        if (itType == _assets.end()) return {};

        auto it = itType->second.find(identifier.name);
        if (it == itType->second.end() || it->second.expired()) return {};

        return it->second.lock();
    }

    void AssetCollection::store(const std::shared_ptr<Asset>& asset,
                                AssetStorageMode mode) {
        if (asset == nullptr) return;
        if (mode == AssetStorageMode::PERMANENT)
            _permanentAssets[asset->getIdentifier()] = asset;
        _assets[asset->getType()][asset->getName()] = asset;
    }

    bool AssetCollection::remove(const AssetIdentifier& identifier) {
        auto itType = _assets.find(identifier.type);
        if (itType == _assets.end()) return false;

        _permanentAssets.erase(identifier);
        return itType->second.erase(identifier.name) == 1;
    }

    bool AssetCollection::remove(const std::shared_ptr<Asset>& asset) {
        if (asset == nullptr) return false;

        auto itType = _assets.find(asset->getType());
        if (itType == _assets.end()) return false;

        _permanentAssets.erase(asset->getIdentifier());
        return itType->second.erase(asset->getName()) == 1;
    }

    void AssetCollection::flushExpiredReferences() {
        for (auto& val: _assets | std::views::values) {
            std::erase_if(val, [](const auto& it) {
                auto const& [key, value] = it;
                return value.expired();
            });
        }
    }
}
