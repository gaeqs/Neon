//
// Created by gaeqs on 02/03/2023.
//

#ifndef NEON_ASSETCOLLECTION_H
#define NEON_ASSETCOLLECTION_H

#include <memory>
#include <unordered_map>
#include <vector>

#include <engine/structure/Asset.h>

namespace neon {

    enum class StorageMode {
        PERMANENT,
        WEAK
    };

    /**
     * A container for all assets inside an application.
     */
    class AssetCollection {

        std::vector<std::shared_ptr<Asset>> _permanentAssets;
        std::unordered_map<AssetIdentifier, std::weak_ptr<Asset>> _assets;


        void refresh();

    };
}

#endif //NEON_ASSETCOLLECTION_H
