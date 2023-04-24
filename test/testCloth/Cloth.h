//
// Created by grial on 17/02/23.
//

#ifndef NEON_CLOTH_H
#define NEON_CLOTH_H

#include "TestVertex.h"
#include "cloth/PhysicsManager.h"
#include "cloth/MassSpring.h"

#include <cstdint>
#include <engine/Engine.h>

class Cloth : public neon::Component {

    std::shared_ptr<neon::Material> _material;
    neon::IdentifiableWrapper<PhysicsManager> _physicsManager;
    uint32_t _width;
    uint32_t _height;

    std::shared_ptr<neon::Model> _model;

    std::vector<TestVertex> _vertices;
    std::vector<TestVertex> _modifiedVertices;

    MassSpring* _massSpring;

    void generateModel();

public:

    Cloth(std::shared_ptr<neon::Material> material,
          neon::IdentifiableWrapper<PhysicsManager> physicsManager,
          uint32_t width, uint32_t height);

    void onStart() override;

    void onPreDraw() override;

};


#endif //NEON_CLOTH_H
