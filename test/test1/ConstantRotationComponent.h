//
// Created by gaelr on 20/11/2022.
//

#ifndef NEON_CONSTANTROTATIONCOMPONENT_H
#define NEON_CONSTANTROTATIONCOMPONENT_H

#include <engine/Engine.h>

class ConstantRotationComponent : public Component {

    float _speed;

public:

    void onStart() override;

    void onUpdate(float deltaTime) override;

};


#endif //NEON_CONSTANTROTATIONCOMPONENT_H
