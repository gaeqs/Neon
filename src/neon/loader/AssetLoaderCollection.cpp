//
// Created by gaeqs on 25/10/2024.
//

#include "AssetLoaderCollection.h"

#include "MaterialLoader.h"


namespace neon {
    AssetLoaderCollection::AssetLoaderCollection(bool loadDefault) {
        if (!loadDefault) return;
        registerLoader<Material, MaterialLoader>();
    }
}
