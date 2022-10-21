//
// Created by grial on 21/10/22.
//

#include "TexturesHolder.h"

TexturesHolder::TexturesHolder() : _textures() {
}

std::weak_ptr<Texture> TexturesHolder::createTexture(
        const std::string& name, const char* data,
        int32_t width, int32_t height) {
    auto texture = std::make_shared<Texture>(data, width, height);
    _textures.try_emplace(name, texture);
    return texture;
}

bool TexturesHolder::hasTexture(const std::string& name) {
    return _textures.contains(name);
}

std::optional<std::weak_ptr<Texture>>
TexturesHolder::getTexture(const std::string& name) {
    auto it = _textures.find(name);
    if (it != _textures.end()) {
        return it->second;
    }
    return {};
}

bool TexturesHolder::removeTexture(const std::string& name) {
    auto it = _textures.find(name);
    if (it != _textures.end()) {
        _textures.erase(it);
        return true;
    }
    return false;
}
