//
// Created by gaelr on 04/11/2022.
//

#include "ShaderUniformBuffer.h"

uint64_t SHADER_UNIFORM_BUFFER_ID_GENERATOR = 1;

ShaderUniformBuffer::ShaderUniformBuffer(
        const std::shared_ptr<ShaderUniformDescriptor>& descriptor) :
        _id(SHADER_UNIFORM_BUFFER_ID_GENERATOR++),
        _implementation(descriptor) {
}

uint64_t ShaderUniformBuffer::getId() const {
    return _id;
}

void ShaderUniformBuffer::setBindingPoint(uint32_t point) {
    _implementation.setBindingPoint(point);
}

void ShaderUniformBuffer::uploadData(uint32_t index,
                                     const void* data, size_t size) {
    _implementation.uploadData(index, data, size);
}

void ShaderUniformBuffer::setTexture(
        uint32_t index,
        IdentifiableWrapper<Texture> texture) {
    _implementation.setTexture(index, texture);
}

void ShaderUniformBuffer::prepareForFrame() {
    _implementation.prepareForFrame();
}

const ShaderUniformBuffer::Implementation&
ShaderUniformBuffer::getImplementation() const {
    return _implementation;
}

ShaderUniformBuffer::Implementation& ShaderUniformBuffer::getImplementation() {
    return _implementation;
}


