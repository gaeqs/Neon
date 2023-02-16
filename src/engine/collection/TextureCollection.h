//
// Created by grial on 21/10/22.
//

#ifndef RVTRACKING_TEXTURECOLLECTION_H
#define RVTRACKING_TEXTURECOLLECTION_H

#include <vector>

#include <cmrc/cmrc.hpp>

#include <engine/collection/IdentifiableCollection.h>
#include <engine/render/Texture.h>

namespace neon {

    class Room;

/**
 * This class holds all textures of a room.
 */
    class TextureCollection : public IdentifiableCollection<Texture> {

    public:

        explicit TextureCollection(Room* room);

        /**
         * Creates a new texture from a resource.
         * @param resource the resource.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        IdentifiableWrapper<Texture> createTextureFromFile(
                const cmrc::file& resource,
                const TextureCreateInfo& createInfo = TextureCreateInfo());

        /**
         * Creates a new texture from a set of resources.
         *
         * This method is useful if you want to create cube boxes
         * or layered textures.
         *
         * All images must have the same size!
         *
         * @param resource the resources.
         * Their raw data will be concatenated.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        IdentifiableWrapper<Texture> createTextureFromFiles(
                const std::vector<cmrc::file>& resources,
                const TextureCreateInfo& createInfo = TextureCreateInfo());

        /**
         * Creates a new texture from a file.
         * @param path the path of the file.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        IdentifiableWrapper<Texture> createTextureFromFile(
                const std::string& path,
                const TextureCreateInfo& createInfo = TextureCreateInfo());

        /**
         * Creates a new texture from a set of files.
         *
         * This method is useful if you want to create cube boxes
         * or layered textures.
         *
         * All images must have the same size!
         *
         * @param paths the paths of the files.
         * Their raw data will be concatenated.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        IdentifiableWrapper<Texture> createTextureFromFiles(
                const std::vector<std::string>& paths,
                const TextureCreateInfo& createInfo = TextureCreateInfo());

        /**
         * Creates a new texture from raw data.
         * @param data the data.
         * @param size the length of the data in bytes.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        IdentifiableWrapper<Texture> createTextureFromFile(
                const void* data, uint32_t size,
                const TextureCreateInfo& createInfo = TextureCreateInfo());

        /**
         * Creates a new texture from a set of raw data pointers.
         *
         * This method is useful if you want to create cube boxes
         * or layered textures.
         *
         * All images must have the same size!
         *
         * @param data the data array.
         * Their raw data will be concatenated.
         * @param sizes the lengths of the data in bytes.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        IdentifiableWrapper<Texture> createTextureFromFile(
                const std::vector<const void*>& data,
                const std::vector<uint32_t>& sizes,
                const TextureCreateInfo& createInfo = TextureCreateInfo());

    };
}


#endif //RVTRACKING_TEXTURECOLLECTION_H
