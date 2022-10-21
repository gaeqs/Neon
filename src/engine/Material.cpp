//
// Created by grial on 19/10/22.
//

#include "Material.h"

#include <gl/Shader.h>

const std::string& Material::getShader() const {
    return _shader;
}

void Material::setShader(const std::string& shader) {
    _shader = shader;
}

void Material::uploadUniforms(Shader& shader) {

    for (const auto& item: _uniformValues) {
        shader.setUniform(
                item.first,
                item.second.floatAmount,
                item.second.floats
        );
    }

}
