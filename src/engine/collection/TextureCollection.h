//
// Created by grial on 21/10/22.
//

#ifndef RVTRACKING_TEXTURECOLLECTION_H
#define RVTRACKING_TEXTURECOLLECTION_H

#include <cmrc/cmrc.hpp>

#include <engine/collection/IdentifiableCollection.h>
#include <engine/render/Texture.h>

class Room;

/**
 * This class holds all textures of a room.
 */
class TextureCollection : public IdentifiableCollection<Texture> {

public:

    explicit TextureCollection(Room* room);

    /**
     * Creates a new texture from a PNG image.
     * @param resource the data of the PNG image.
     * @return a pointer to the new texture.
     */
    IdentifiableWrapper<Texture> createTextureFromPNG(
            const cmrc::file& resource);

    /**
     * Creates a new texture from a PNG image.
     * @param data the data of the PNG image.
     * @param size the length of the data array in bytes.
     * @return a pointer to the new texture.
     */
    IdentifiableWrapper<Texture> createTextureFromPNG(
            const void* data, uint32_t size);

};


#endif //RVTRACKING_TEXTURECOLLECTION_H
