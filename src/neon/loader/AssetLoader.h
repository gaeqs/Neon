//
// Created by gaeqs on 25/10/24.
//

#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H
#include <neon/filesystem/FileSystem.h>
#include <neon/structure/Application.h>
#include <neon/structure/collection/AssetCollection.h>

namespace neon {
    class AssetLoader {
        Application* _application;
        FileSystem* _fileSystem;
        AssetCollection* _assetCollection;

    public:
        AssetLoader(Application* application, FileSystem* fileSystem, AssetCollection* assetCollection);

        void loadAsset(std::filesystem::path path);
    };
}


#endif //RESOURCELOADER_H
