//
// Created by grial on 21/10/22.
//

#include "TextureCollection.h"

TextureCollection::TextureCollection() : _textures() {
}

std::weak_ptr<Texture> TextureCollection::createTexture(
        const std::string& name, const char* data,
        int32_t width, int32_t height) {
    auto texture = std::make_shared<Texture>(data, width, height);
    _textures.try_emplace(name, texture);
    return texture;
}

bool TextureCollection::hasTexture(const std::string& name) {
    return _textures.contains(name);
}

std::optional<std::weak_ptr<Texture>>
TextureCollection::getTexture(const std::string& name) {
    auto it = _textures.find(name);
    if (it != _textures.end()) {
        return it->second;
    }
    return {};
}

bool TextureCollection::removeTexture(const std::string& name) {
    auto it = _textures.find(name);
    if (it != _textures.end()) {
        _textures.erase(it);
        return true;
    }
    return false;
}
