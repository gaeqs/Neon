//
// Created by grial on 17/02/23.
//

#include "Cloth.h"

#include <cstdint>
#include <memory>
#include <stdint.h>
#include <vector>

#include <glm/glm.hpp>

#include "TestVertex.h"
#include "engine/structure/IdentifiableWrapper.h"


void Cloth::generateModel() {
    _vertices.reserve(_width * _height);

    std::vector<glm::vec3> positions;
    positions.reserve(_width * _height);

    auto w = static_cast<float>(_width);
    auto h = static_cast<float>(_height);

    for (float x = 0; x < w; x += 1.0f) {
        for (float z = 0; z < h; z += 1.0f) {
            _vertices.push_back(TestVertex{
                    glm::vec3(x, 0.0f, z),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec2(x / w, z / h)
            });
            positions.emplace_back(x, 0.0f, z);
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
            indices.push_back(v3);
            indices.push_back(v2);
            indices.push_back(v0);
            indices.push_back(v1);
            indices.push_back(v3);
        }
    }

    std::vector<std::unique_ptr<neon::Mesh>> meshes;
    meshes.push_back(std::make_unique<neon::Mesh>(
            getRoom(),
            _material,
            true,
            false
    ));
    meshes[0]->reinitializeVertexData(_vertices, indices);

    _model = getRoom()->getModels().create(meshes);

    _massSpring = _physicsManager->createSimulableObject<MassSpring>(
            getGameObject(),
            positions,
            indices,
            500.0f,
            1000.0f,
            10.0f,
            0.5f,
            0.005f
    );

    getGameObject()->newComponent<neon::GraphicComponent>(_model);
}

Cloth::Cloth(neon::IdentifiableWrapper<neon::Material> material,
             neon::IdentifiableWrapper<PhysicsManager> physicsManager,
             uint32_t width, uint32_t height) :
        _material(material),
        _physicsManager(physicsManager),
        _width(width),
        _height(height),
        _model(),
        _vertices(),
        _modifiedVertices(),
        _massSpring() {

}

void Cloth::onStart() {
    generateModel();
}

void Cloth::onPreDraw() {
    _modifiedVertices.resize(_vertices.size());

    uint32_t i = 0;
    for (const auto& node: _massSpring->getNodes()) {
        TestVertex vertex = _vertices[i];

        auto& pos = node.getPositionVector();
        vertex.position = {pos.x(), pos.y(), pos.z()};

        _modifiedVertices[i] = vertex;
        ++i;
    }

    _model->getMesh(0)->setVertices(_modifiedVertices);
}
