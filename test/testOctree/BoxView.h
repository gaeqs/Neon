//
// Created by grial on 17/02/23.
//

#ifndef NEON_LINE_H
#define NEON_LINE_H

#include "TestVertex.h"

#include <cstdint>
#include <utility>
#include <neon/Neon.h>

#include <rush/rush.h>

class BoxView : public neon::Component {
    std::shared_ptr<neon::Material> _material;
    std::shared_ptr<neon::Model> _model;

    rush::AABB<3, float> _box;
    float _index;

    void generateModel() {
        neon::ModelCreateInfo info;
        info.drawables.push_back(std::make_unique<neon::Mesh>(
            getApplication(),
            "box",
            _material,
            false,
            false
        ));


        // Generate vertices
        std::vector<TestVertex> vertices;

        auto min = _box.center + _box.radius;
        auto max = _box.center - _box.radius;

        rush::Vec<3, float> edges[2] = {min, max};
        vertices.reserve(8);

        for (size_t i = 0; i < 8; ++i) {
            vertices.push_back({
                {
                    edges[i & 1].x(),
                    edges[i >> 1 & 1].y(),
                    edges[i >> 2 & 1].z()
                },
                _index
            });
        }

        // 2 ------ 3
        // |        |
        // |        |
        // 0 ------ 1

        const std::vector<uint32_t> indices = {
            // Bottom
            0, 1,
            0, 2,
            1, 3,
            2, 3,

            // Top
            4, 5,
            4, 6,
            5, 7,
            6, 8,

            // Vertical
            0, 4,
            1, 5,
            2, 6,
            3, 7,
        };


        info.drawables[0]->setMeshData(vertices, indices);
        info.maximumInstances = 1;

        _model = std::make_shared<neon::Model>(getApplication(), "line", info);

        getGameObject()->newComponent<neon::GraphicComponent>(_model);
    }

public:
    BoxView(std::shared_ptr<neon::Material> material,
        const rush::AABB<3, float>& box,
        float index)
        : _material(std::move(material)),
          _box(box),
          _index(index) {
    }

    void onStart() override {
        generateModel();
    }
};


#endif //NEON_LINE_H
