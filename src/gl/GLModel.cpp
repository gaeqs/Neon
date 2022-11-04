//
// Created by gaelr on 03/11/2022.
//

#include "GLModel.h"

#include <iostream>
#include <glad/glad.h>

#include <engine/model/DefaultInstancingData.h>
#include <gl/Shader.h>

void GLModel::reinitializeBuffer() const {
    glBindBuffer(GL_ARRAY_BUFFER, _instancingBuffer);
    glBufferData(GL_ARRAY_BUFFER, _instancingStructSize * BUFFER_DEFAULT_SIZE,
                 nullptr, GL_DYNAMIC_DRAW);
}

GLModel::GLModel(std::vector<GLMesh*> meshes) :
        _meshes(std::move(meshes)),
        _instancingBuffer(0),
        _instancingStructType(std::type_index(typeid(DefaultInstancingData))),
        _instancingStructSize(sizeof(DefaultInstancingData)),
        _positions() {
    glGenBuffers(1, &_instancingBuffer);
    reinitializeBuffer();
    for (const auto& mesh: _meshes) {
        mesh->configureInstancingBuffer<DefaultInstancingData>(_instancingBuffer);
    }
}

GLModel::~GLModel() {
    glad_glDeleteBuffers(1, &_instancingBuffer);
}

const std::type_index& GLModel::getInstancingStructType() const {
    return _instancingStructType;
}

Result<uint32_t*, std::string> GLModel::createInstance() {
    if (_positions.size() >= BUFFER_DEFAULT_SIZE) {
        return {"Buffer is full"};
    }

    auto* id = new uint32_t(_positions.size());
    _positions.push_back(id);
    return {id};
}

bool GLModel::freeInstance(uint32_t id) {
    if (id >= _positions.size()) return false;

    auto* toRemove = _positions[id];
    auto* last = _positions.back();

    if (_instancingStructSize != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, _instancingBuffer);
        glCopyBufferSubData(
                GL_ARRAY_BUFFER,
                GL_ARRAY_BUFFER,
                *last * _instancingStructSize,
                id * _instancingStructSize,
                _instancingStructSize
        );
    }

    _positions[id] = last;
    _positions.pop_back();
    *last = id;

    delete toRemove;

    return true;
}

void GLModel::uploadDataRaw(uint32_t id, const void* raw) const {
    if (_instancingStructSize == 0) {
        std::cerr << "[OPENGL MODEL] Cannot upload data to buffer "
                  << _instancingBuffer << ": instance struct is not defined!"
                  << std::endl;
        return;
    }
    if (id >= _positions.size()) {
        std::cerr << "[OPENGL MODEL] Cannot upload data to buffer "
                  << _instancingBuffer << ": invalid id " << id << "!"
                  << std::endl;
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, _instancingBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, _instancingStructSize * id,
                    _instancingStructSize, raw);
}

void GLModel::draw(Shader* shader, TextureCollection& textures) const {
    shader->use();
    for (const auto& item: _meshes) {
        item->draw(shader, textures, _positions.size());
    }
}
