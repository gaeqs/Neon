//
// Created by grial on 17/02/23.
//

#include "Cloth.h"

#include <cstdint>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "TestVertex.h"

void Cloth::generateModel() {
    std::vector<TestVertex> vertices;
    vertices.reserve(_width * _height);

    auto w = static_cast<float>(_width);
    auto h = static_cast<float>(_height);

    for (float x = 0; x < w; x += 1.0f) {
        for (float z = 0; z < h; z += 1.0f) {
            vertices.push_back(TestVertex{
                    glm::vec3(x, 0.0f, z),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec2(x / w, z / h)
            });
        }
    }

    std::vector<uint32_t> indices;
    indices.reserve((_width - 1) * (_height - 1) * 2);

    for (uint32_t x = 0; x < _width - 1; ++x) {
        for (uint32_t z = 0; z < _height - 1; ++z) {
            uint32_t v0 = x + z * _width;
            uint32_t v1 = x + 1 + z * _width;
            uint32_t v2 = x + (z + 1) * _width;
            uint32_t v3 = x + 1 + (z + 1) * _width;

            indices.push_back(v0);
            indices.push_back(v2);
            indices.push_back(v3);
            indices.push_back(v0);
            indices.push_back(v3);
            indices.push_back(v1);
        }
    }

    std::vector<std::unique_ptr<neon::Mesh>> meshes;
    meshes.push_back(std::make_unique<neon::Mesh>(
            getRoom(),
            _material,
            true,
            false
    ));

    _model = getRoom()->getModels().create(meshes);

    getGameObject()->newComponent<neon::GraphicComponent>(_model);
}

void Cloth::onStart() {


}

Cloth::Cloth(neon::IdentifiableWrapper<neon::Material> material,
             uint32_t width, uint32_t height) :
        _material(material),
        _width(width),
        _height(height),
        _model() {

}
