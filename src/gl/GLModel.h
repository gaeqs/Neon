//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_GLMODEL_H
#define RVTRACKING_GLMODEL_H


#include <cstdint>
#include <vector>

#include <glad/glad.h>

class GLModel {

    uint32_t _vao;
    uint32_t _vbo;
    uint32_t _ebo;

    uint32_t _vertexAmount;
    uint32_t _indexAmount;

public:

    GLModel(const GLModel& other) = delete;

    template<class Vertex>
    GLModel(std::vector<Vertex> vertices, std::vector<uint32_t> indices):
            _vao(0),
            _vbo(0),
            _ebo(0),
            _vertexAmount(vertices.size()),
            _indexAmount(indices.size()) {
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ebo);

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
                     vertices.data(),GL_STATIC_DRAW);

        Vertex::setupVAO();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);
    }

    ~GLModel();

    uint32_t getVao() const;

    uint32_t getVbo() const;

    uint32_t getEbo() const;

    uint32_t getVertexAmount() const;

    uint32_t getIndexAmount() const;

    void draw() const;
};


#endif //RVTRACKING_GLMODEL_H
