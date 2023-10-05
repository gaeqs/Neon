//
// Created by grial on 17/02/23.
//

#ifndef NEON_LINE_H
#define NEON_LINE_H

#include "TestVertex.h"

#include <cstdint>
#include <engine/Engine.h>

#include <rush/rush.h>

class Line : public neon::Component {

    std::shared_ptr<neon::Material> _material;
    std::shared_ptr<neon::Model> _model;

    std::vector<rush::Vec3f> _points;

    void generateModel();

public:

    Line(std::shared_ptr<neon::Material> material,
         const std::vector<rush::Vec3f>& points);

    void onStart() override;

};


#endif //NEON_LINE_H
