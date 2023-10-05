//
// Created by grial on 17/02/23.
//

#include "Line.h"

#include <cstdint>
#include <memory>
#include <stdint.h>
#include <utility>
#include <vector>

#include <rush/rush.h>

#include "TestVertex.h"
#include "engine/structure/IdentifiableWrapper.h"


void Line::generateModel() {
    neon::ModelCreateInfo info;
    info.meshes.push_back(std::make_unique<neon::Mesh>(
            getApplication(),
            "line",
            _material,
            false,
            false
    ));

    std::vector<uint32_t> indices;
    indices.reserve(_points.size());

    for (uint32_t i = 0; i < _points.size(); ++i) {
        indices.push_back(i);
    }

    info.meshes[0]->setMeshData(_points, indices);
    info.maximumInstances = 1;

    _model = std::make_shared<neon::Model>(getApplication(), "line", info);

    getGameObject()->newComponent<neon::GraphicComponent>(_model);
    _points.clear();
}

Line::Line(std::shared_ptr<neon::Material> material,
           const std::vector<rush::Vec3f>& points) :
        _material(std::move(material)),
        _model(),
        _points(points) {

}

void Line::onStart() {
    generateModel();
}