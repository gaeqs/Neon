//
// Created by gaelr on 23/10/2022.
//

#include "GLModel.h"

GLModel::~GLModel() {
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteVertexArrays(1, &_vao);
}

uint32_t GLModel::getVao() const {
    return _vao;
}

uint32_t GLModel::getVbo() const {
    return _vbo;
}

uint32_t GLModel::getEbo() const {
    return _ebo;
}

uint32_t GLModel::getVertexAmount() const {
    return _vertexAmount;
}

uint32_t GLModel::getIndexAmount() const {
    return _indexAmount;
}

void GLModel::draw() const {
    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glDrawElements(GL_TRIANGLES, static_cast<int>(_indexAmount),
                   GL_UNSIGNED_INT, 0);
}
