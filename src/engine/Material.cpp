//
// Created by grial on 19/10/22.
//

#include "Material.h"

#include <cstring>

#include <engine/Texture.h>
#include <engine/Room.h>

const std::string& Material::getShader() const {
    return _shader;
}

void Material::setShader(const std::string& shader) {
    _shader = shader;
}

const std::unordered_map<std::string, MaterialEntry>&
Material::getUniformValues() const {
    return _uniformValues;
}

void Material::setImage(const std::string& name, uint64_t imageId) {
    std::vector<char> data;
    data.resize(sizeof(uint64_t));
    memcpy(data.data(), &imageId, sizeof(uint64_t));
    _uniformValues[name] = {MaterialEntryType::IMAGE, data};
}

void Material::setImage(const std::string& name,
                        IdentifiableWrapper<Texture> image) {
    setImage(name, image->getId());
}
