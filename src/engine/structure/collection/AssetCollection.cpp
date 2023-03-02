//
// Created by gaeqs on 02/03/2023.
//

#include "AssetCollection.h"
#include "engine/structure/Asset.h"

void neon::AssetCollection::refresh() {
    std::erase_if(_assets, [](const auto& it) {
        auto const& [key, value] = it;
        return value.expired();
    });
}
