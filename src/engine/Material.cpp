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

void Material::setImage(const std::string& name, uint64_t imageId,
                        uint32_t target) {
    std::vector<char> data;
    data.resize(sizeof(uint64_t) * 2);
    uint64_t raw[2] = {imageId, target};
    memcpy(data.data(), &raw, sizeof(uint64_t) * 2);
    _uniformValues[name] = {MaterialEntryType::IMAGE, data};
}

void Material::setImage(const std::string& name,
                        IdentifiableWrapper<Texture> image,
                        uint32_t target) {
    setImage(name, image->getId(), target);
}
