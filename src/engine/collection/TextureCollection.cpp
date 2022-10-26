//
// Created by grial on 21/10/22.
//

#include "TextureCollection.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

TextureCollection::TextureCollection() : _textures() {
}

IdentifiableWrapper<Texture> TextureCollection::createTexture(
        const char* data, int32_t width, int32_t height) {
    auto value = IdentifiableWrapper<Texture>(
            _textures.emplace(data, width, height));
    _texturesById.emplace(value->getId(), value);
    return value;
}

IdentifiableWrapper<Texture>
TextureCollection::createTextureFromPNG(const cmrc::file& resource) {
    auto pngPtr = (const unsigned char*) resource.begin();
    auto size = static_cast<int32_t>(resource.size());
    int32_t width, height, channels;
    auto ptr = stbi_load_from_memory(
            pngPtr, size, &width, &height, &channels, 4);
    auto image = createTexture((const char*) ptr, width, height);
    stbi_image_free(ptr);
    return image;
}

IdentifiableWrapper<Texture>
TextureCollection::getTexture(uint64_t id) const {
    auto it = _texturesById.find(id);
    if (it != _texturesById.end()) {
        return it->second;
    }
    return nullptr;
}

bool TextureCollection::destroyTexture(
        const IdentifiableWrapper<Texture>& texture) {
    _texturesById.erase(texture->getId());
    return _textures.remove(texture.raw());
}
