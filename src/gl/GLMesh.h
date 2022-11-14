//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_GLMODEL_H
#define RVTRACKING_GLMODEL_H


#include <cstdint>
#include <vector>

#include <glad/glad.h>
#include <engine/Material.h>

class Shader;

class TextureCollection;

class Application;

class GLMesh {

    uint32_t _vao;
    uint32_t _vbo;
    uint32_t _ebo;

    uint32_t _vertexAmount;
    uint32_t _indexAmount;
    uint32_t _attributeAmount;

    Material& _material;

public:

    GLMesh(const GLMesh& other) = delete;

    explicit GLMesh(Application* application, Material& material);

    ~GLMesh();

    [[nodiscard]] uint32_t getVao() const;

    [[nodiscard]] uint32_t getVbo() const;

    [[nodiscard]] uint32_t getEbo() const;

    [[nodiscard]] uint32_t getVertexAmount() const;

    [[nodiscard]] uint32_t getIndexAmount() const;

    [[nodiscard]] uint32_t getAttributeAmount() const;

    template<class Vertex>
    void uploadData(std::vector<Vertex> vertices,
                    std::vector<uint32_t> indices) {
        _vertexAmount = vertices.size();
        _indexAmount = indices.size();

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);

        _attributeAmount = Vertex::setupVAO();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);
    }

    template<class InstanceData>
    void configureInstancingBuffer(uint32_t buffer) {
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        InstanceData::setupInstancingVAO(_attributeAmount);
    }

    void draw(Shader* shader, TextureCollection& textures,
              uint32_t instances) const;
};


#endif //RVTRACKING_GLMODEL_H
