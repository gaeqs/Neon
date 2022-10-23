//
// Created by grial on 21/10/22.
//

#include "TextureCollection.h"

TextureCollection::TextureCollection() : _textures() {
}

IdentifiableWrapper<Texture> TextureCollection::createTexture(
        const char* data, int32_t width, int32_t height) {
    auto value = IdentifiableWrapper<Texture>(
            _textures.emplace(data, width, height));
    _texturesById.emplace(value.isValid(), value);
    return value;
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
