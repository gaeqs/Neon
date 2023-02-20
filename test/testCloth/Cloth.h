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

    neon::IdentifiableWrapper<neon::Material> _material;
    neon::IdentifiableWrapper<PhysicsManager> _physicsManager;
    uint32_t _width;
    uint32_t _height;

    neon::IdentifiableWrapper<neon::Model> _model;

    std::vector<TestVertex> _vertices;
    std::vector<TestVertex> _modifiedVertices;

    MassSpring* _massSpring;

    void generateModel();

public:

    Cloth(neon::IdentifiableWrapper<neon::Material> material,
          neon::IdentifiableWrapper<PhysicsManager> physicsManager,
          uint32_t width, uint32_t height);

    void onStart() override;

    void onPreDraw() override;

};


#endif //NEON_CLOTH_H
