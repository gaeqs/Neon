//
// Created by grial on 19/10/22.
//

#include "Material.h"

uint64_t MATERIAL_ID_GENERATOR = 1;

Material::Material(
        Room* room,
        IdentifiableWrapper<ShaderProgram> shader,
        const std::shared_ptr<ShaderUniformDescriptor>& descriptor,
        const InputDescription& vertexDescription,
        const InputDescription& instanceDescription) :
        _id(MATERIAL_ID_GENERATOR++),
        _shader(shader),
        _uniformDescriptor(descriptor),
        _uniformBuffer(descriptor),
        _implementation(room, this, vertexDescription, instanceDescription) {

}

uint64_t Material::getId() const {
    return _id;
}

const IdentifiableWrapper<ShaderProgram>& Material::getShader() const {
    return _shader;
}

const ShaderUniformBuffer& Material::getUniformBuffer() const {
    return _uniformBuffer;
}

ShaderUniformBuffer& Material::getUniformBuffer() {
    return _uniformBuffer;
}

const std::shared_ptr<ShaderUniformDescriptor>&
Material::getUniformDescriptor() const {
    return _uniformDescriptor;
}

const Material::Implementation& Material::getImplementation() const {
    return _implementation;
}

Material::Implementation& Material::getImplementation() {
    return _implementation;
}
