//
// Created by grial on 19/10/22.
//

#include "Material.h"

#include <utility>

uint64_t MATERIAL_ID_GENERATOR = 1;

Material::Material(
        Room* room,
        const std::shared_ptr<FrameBuffer>& target,
        IdentifiableWrapper<ShaderProgram> shader,
        const std::shared_ptr<ShaderUniformDescriptor>& descriptor,
        const InputDescription& vertexDescription,
        const InputDescription& instanceDescription,
        MaterialConfiguration configuration) :
        _id(MATERIAL_ID_GENERATOR++),
        _shader(shader),
        _uniformDescriptor(descriptor),
        _uniformBuffer(descriptor),
        _implementation(room, this, target, vertexDescription,
                        instanceDescription, configuration) {
    _uniformBuffer.setBindingPoint(1);
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

void Material::pushConstant(
        const std::string& name, const void* data, uint32_t size) {
    _implementation.pushConstant(name, data, size);
}

void Material::setTexture(const std::string& name,
                          IdentifiableWrapper<Texture> texture) {
    _implementation.setTexture(name, texture);
}