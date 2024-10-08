//
// Created by grial on 17/02/23.
//

#ifndef NEON_LINE_H
#define NEON_LINE_H

#include "TestVertex.h"

#include <cstdint>
#include <neon/Neon.h>

#include <rush/rush.h>

template<typename Curve>
class Box : public neon::Component {

    std::shared_ptr<neon::Material> _material;
    std::shared_ptr<neon::Model> _model;

    uint32_t _samples;
    Curve _curve;

    void generateModel() {
        neon::ModelCreateInfo info;
        info.drawables.push_back(std::make_unique<neon::Mesh>(
                getApplication(),
                "line",
                _material,
                false,
                false
        ));


        // Generate vertices
        std::vector<TestVertex> vertices;
        std::vector<uint32_t> indices;

        vertices.reserve(_samples);
        indices.reserve(_samples);

        float d = 1.0f / static_cast<float>(_samples - 1);


        for (uint32_t i = 0; i < _samples - 1; ++i) {
            float t = d * static_cast<float>(i);
            vertices.push_back(TestVertex(_curve.fetch(t), t));
            indices.push_back(i);
        }

        vertices.push_back(TestVertex(_curve.fetch(1.0f), 1.0f));
        indices.push_back(_samples - 1);


        info.drawables[0]->uploadVertices(vertices);
        info.drawables[0]->uploadIndices(indices);
        info.maximumInstances = 1;

        _model = std::make_shared<neon::Model>(getApplication(), "line", info);

        getGameObject()->template newComponent<neon::GraphicComponent>(_model);
    }

public:

    Box(std::shared_ptr<neon::Material> material, Curve curve,
         uint32_t samples) :
            _material(material),
            _model(),
            _curve(curve),
            _samples(samples) {

    }

    void onStart() override {
        generateModel();
    }

};


#endif //NEON_LINE_H
