//
// Created by grial on 17/02/23.
//

#include "Cloth.h"

#include <cstdint>
#include <memory>
#include <stdint.h>
#include <vector>

#include <rush/rush.h>

#include "TestVertex.h"
#include "neon/structure/IdentifiableWrapper.h"

void Cloth::generateModel()
{
    _vertices.reserve(_width * _height);

    std::vector<rush::Vec3f> positions;
    positions.reserve(_width * _height);

    auto w = static_cast<float>(_width);
    auto h = static_cast<float>(_height);

    for (float x = 0; x < w; x += 1.0f) {
        for (float z = 0; z < h; z += 1.0f) {
            _vertices.push_back(TestVertex{rush::Vec3f(x, 0.0f, z), rush::Vec3f(0.0f, 1.0f, 0.0f),
                                           rush::Vec3f(1.0f, 0.0f, 0.0f), rush::Vec2f(x / w, z / h)});
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

    auto mesh = std::make_unique<neon::Mesh>(getApplication(), "cloth", _material, true, false);

    mesh->uploadVertices(_vertices);
    mesh->uploadIndices(indices);

    neon::ModelCreateInfo info;
    info.drawables.push_back(std::move(mesh));

    info.maximumInstances = 1;

    _model = std::make_shared<neon::Model>(getApplication(), "cloth", info);

    _massSpring = _physicsManager->createSimulableObject<MassSpring>(getGameObject(), positions, indices, 500.0f,
                                                                     1000.0f, 10.0f, 0.5f, 0.005f);

    getGameObject()->newComponent<neon::GraphicComponent>(_model);
}

Cloth::Cloth(std::shared_ptr<neon::Material> material, neon::IdentifiableWrapper<PhysicsManager> physicsManager,
             uint32_t width, uint32_t height) :
    _material(material),
    _physicsManager(physicsManager),
    _width(width),
    _height(height),
    _model(),
    _vertices(),
    _modifiedVertices(),
    _massSpring()
{
}

void Cloth::onStart()
{
    generateModel();
}

void Cloth::onPreDraw()
{
    _modifiedVertices.resize(_vertices.size());

    uint32_t i = 0;
    for (const auto& node : _massSpring->getNodes()) {
        TestVertex vertex = _vertices[i];

        auto& pos = node.getPositionVector();
        vertex.position = {pos.x(), pos.y(), pos.z()};

        _modifiedVertices[i] = vertex;
        ++i;
    }

    reinterpret_cast<neon::Mesh*>(_model->getDrawable(0))->setVertices(0, _modifiedVertices);
}
