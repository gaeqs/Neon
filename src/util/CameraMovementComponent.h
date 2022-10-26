//
// Created by gaelr on 26/10/2022.
//

#ifndef VRTRACKING_CAMERAMOVEMENTCOMPONENT_H
#define VRTRACKING_CAMERAMOVEMENTCOMPONENT_H

#include <engine/Component.h>

class CameraMovementComponent : public Component {

    bool _w, _a, _s, _d, _shift, _space;
    float _speed;

public:

    CameraMovementComponent();

    void onKey(int32_t key, int32_t scancode,
               int32_t action, int32_t mods) override;

    void onUpdate(float deltaTime) override;

    float getSpeed() const;

    void setSpeed(float speed);
};


#endif //VRTRACKING_CAMERAMOVEMENTCOMPONENT_H
