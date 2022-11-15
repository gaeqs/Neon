//
// Created by gaelr on 23/10/2022.
//

#include "GLMesh.h"

#include <engine/Application.h>
#include <engine/collection/TextureCollection.h>
#include <gl/GLShaderProgram.h>

GLMesh::GLMesh([[maybe_unused]] Application* application, Material& material) :
        _vao(0),
        _vbo(0),
        _ebo(0),
        _vertexAmount(0),
        _indexAmount(0),
        _material(material) {
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
}

GLMesh::~GLMesh() {
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteVertexArrays(1, &_vao);
}

uint32_t GLMesh::getVao() const {
    return _vao;
}

uint32_t GLMesh::getVbo() const {
    return _vbo;
}

uint32_t GLMesh::getEbo() const {
    return _ebo;
}

uint32_t GLMesh::getVertexAmount() const {
    return _vertexAmount;
}

uint32_t GLMesh::getIndexAmount() const {
    return _indexAmount;
}

uint32_t GLMesh::getAttributeAmount() const {
    return _attributeAmount;
}

void GLMesh::draw(GLShaderProgram* shader, TextureCollection& textures,
                  uint32_t instances) const {

    uint32_t textureTarget = 0;
    for (const auto& item: _material.getUniformValues()) {
        auto& entry = item.second;
        if (entry.type == MaterialEntryType::IMAGE) {
            auto id = *reinterpret_cast<const uint64_t*>(entry.data.data());
            auto texture = textures.get(id);
            if (texture.isValid()) {
                glActiveTexture(GL_TEXTURE0 + textureTarget);
                glBindTexture(GL_TEXTURE_2D,
                              texture->getImplementation().getId());
                shader->setUniform(item.first, textureTarget);
                ++textureTarget;
            }
        } else {
            shader->setUniform(
                    item.first,
                    static_cast<int32_t>(entry.data.size() >> 2),
                    reinterpret_cast<const float*>(item.second.data.data())
            );
        }
    }

    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

    glDrawElementsInstanced(
            GL_TRIANGLES,
            static_cast<int>(_indexAmount), GL_UNSIGNED_INT,
            nullptr,
            static_cast<int>(instances)
    );
}
