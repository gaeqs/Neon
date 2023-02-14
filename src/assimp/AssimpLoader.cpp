//
// Created by gaeqs on 14/02/2023.
//

#include "AssimpLoader.h"

#include <assimp/postprocess.h>
#include <stdint.h>


namespace assimp_loader {

    // Private methods
    namespace {
        uint32_t decodeFlags(const LoaderInfo& info) {
            uint32_t flags = aiProcess_Triangulate |
                             aiProcess_JoinIdenticalVertices |
                             aiProcess_SortByPType |
                             aiProcess_RemoveRedundantMaterials |
                             aiProcess_EmbedTextures;
            if (info.flipUVs) flags |= aiProcess_FlipUVs;
            if (info.flipWindingOrder) flags |= aiProcess_FlipWindingOrder;
            return flags;
        }
    }

    Result load(const std::string& directory,
                const std::string& file,
                const LoaderInfo& info) {
        Assimp::Importer importer;

        auto previous = std::filesystem::current_path();
        importer.GetIOHandler()->ChangeDirectory(directory);
        auto scene = importer.ReadFile(file, decodeFlags(info));
        std::filesystem::current_path(previous);

        return load(scene, info);
    }

    Result load(const void* buffer,
                size_t length,
                const LoaderInfo& info) {
        Assimp::Importer importer;
        auto scene = importer.ReadFileFromMemory(
                buffer, length, decodeFlags(info));

        return load(scene, info);
    }

    Result load(const aiScene* scene,
                const LoaderInfo& info) {
        if (!scene) return {false};

        // Init collections
        std::map<std::string, IdentifiableWrapper<Texture>> textures;
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<IdentifiableWrapper<Material>> materials;
        meshes.reserve(scene->mNumMeshes);
        materials.reserve(scene->mNumMaterials);

    }
}