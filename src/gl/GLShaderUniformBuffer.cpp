//
// Created by gaelr on 04/11/2022.
//

#include "GLShaderUniformBuffer.h"

#include <glad/glad.h>

GLShaderUniformBuffer::GLShaderUniformBuffer() : _id(0) {
    glGenBuffers(1, &_id);
}

GLShaderUniformBuffer::~GLShaderUniformBuffer() {
    glDeleteBuffers(1, &_id);
}

void GLShaderUniformBuffer::setBindingPoint(uint32_t point) const {
    glBindBufferBase(GL_UNIFORM_BUFFER, point, _id);
}

void GLShaderUniformBuffer::uploadData(const void* data, size_t size) const {
    glBindBuffer(GL_UNIFORM_BUFFER, _id);
    glBufferData(GL_UNIFORM_BUFFER, static_cast<int32_t>(size),
                 data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
