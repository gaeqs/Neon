//
// Created by gaelr on 04/11/2022.
//

#include "ShaderUniformBuffer.h"

uint64_t SHADER_UNIFORM_BUFFER_ID_GENERATOR = 1;

ShaderUniformBuffer::ShaderUniformBuffer() :
        _id(SHADER_UNIFORM_BUFFER_ID_GENERATOR++),
        _implementation() {
}

uint64_t ShaderUniformBuffer::getId() const {
    return _id;
}

void ShaderUniformBuffer::setBindingPoint(uint32_t point) const {
    _implementation.setBindingPoint(point);
}

void ShaderUniformBuffer::uploadData(const void* data, size_t size) const {
    _implementation.uploadData(data, size);
}


