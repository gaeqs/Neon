//
// Created by gaeqs on 14/02/2023.
//

#ifndef NEON_ASSIMPLOADER_H
#define NEON_ASSIMPLOADER_H

#include "ModelLoader.h"
#include "assimp/scene.h"

#include <functional>

#include <engine/structure/IdentifiableWrapper.h>

class Model;

class Room;

class MaterialCreateInfo;

struct aiScene;

namespace assimp_loader {

    struct Result {

        /**
         * Whether the result is valid.
         */
        bool valid = false;

        /**
         * The loaded model.
         * This value is nullptr if the result is not valid.
         */
        IdentifiableWrapper<Model> model = nullptr;
    };

    struct LoaderInfo {

        /**
         * The room where the model is loaded.
         */
        Room* room;

        /**
         * The structure used to create materials.
         * The vertex descriptor will be overridden.
         */
        MaterialCreateInfo materialCreateInfo;

        /**
         * Whether UVs should be flipped.
         */
        bool flipUVs = true;

        /**
         * Whether winding order should be flipped.
         */
        bool flipWindingOrder = false;

        /**
         * Whether normals should be flipped.
         */
        bool flipNormals = false;

        explicit LoaderInfo(Room* room_) : room(room_) {}
    };

    /**
     * Loads the model located at the given file.
     *
     * Always use direct file directions for models
     * codified in separated files!
     *
     * @param directory the directory where the file is located at.
     * @param file the file where the model is codified on.
     * @param info the information about the loading process.
     * @return the model.
     */
    Result load(const std::string& directory,
                const std::string& file,
                const LoaderInfo& info);

    /**
     * Loads the model codified in the given buffer.
     * @param buffer the buffer's pointer.
     * @param length the buffer's size.
     * @param info the information about the loading process.
     * @return the model.
     */
    Result load(const void* buffer,
                size_t length,
                const LoaderInfo& info);

    Result load(const aiScene* scene,
                const LoaderInfo& info);
}


#endif //NEON_ASSIMPLOADER_H
