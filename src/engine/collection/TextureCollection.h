//
// Created by grial on 21/10/22.
//

#ifndef RVTRACKING_TEXTURECOLLECTION_H
#define RVTRACKING_TEXTURECOLLECTION_H

#include <string>
#include <memory>
#include <optional>
#include <unordered_map>

#include "engine/Texture.h"
#include "engine/IdentifiableWrapper.h"
#include "util/ClusteredLinkedCollection.h"

class Resource;

/**
 * This class holds all textures of a room.
 */
class TextureCollection {

    ClusteredLinkedCollection<Texture> _textures;
    std::unordered_map<uint64_t, IdentifiableWrapper<Texture>> _texturesById;

public:

    /**
     * Creates the collection.
     */
    TextureCollection();

    /**
     * Creates a new texture.
     * @param data the data of the texture.
     * @param width the width of the texture.
     * @param height the height of the texture.
     * @return a pointer to the new texture.
     */
    IdentifiableWrapper<Texture> createTexture(
            const char* data, int32_t width, int32_t height);

    /**
     * Creates a new texture from a PNG image.
     * @param data the data of the PNG image.
     * @return a pointer to the new texture.
     */
    IdentifiableWrapper<Texture> createTextureFromPNG(const Resource& resource);

    /**
     * Returns a pointer to the texture that matches the given id.
     * @param id the id.
     * @return the pointer to the texture.
     */
    IdentifiableWrapper<Texture> getTexture(uint64_t id) const;

    /**
     * Destroy the given texture.
     *
     * All pointers to the given texture will be invalid after this call.
     *
     * @param texture the texture.
     * @return whether the operation was successful or not.
     */
    bool destroyTexture(const IdentifiableWrapper<Texture>& texture);
};


#endif //RVTRACKING_TEXTURECOLLECTION_H
