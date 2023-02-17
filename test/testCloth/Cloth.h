//
// Created by grial on 17/02/23.
//

#ifndef NEON_CLOTH_H
#define NEON_CLOTH_H

#include <cstdint>
#include <engine/Engine.h>

class Cloth : public neon::Component {

    neon::IdentifiableWrapper<neon::Material> _material;
    uint32_t _width;
    uint32_t _height;

    neon::IdentifiableWrapper<neon::Model> _model;

    void generateModel();

public:

    Cloth(neon::IdentifiableWrapper<neon::Material> material,
          uint32_t width, uint32_t height);

    void onStart() override;


};


#endif //NEON_CLOTH_H
