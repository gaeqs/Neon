//
// Created by gaelr on 26/10/2022.
//

#ifndef VRTRACKING_CAMERAMOVEMENTCOMPONENT_H
#define VRTRACKING_CAMERAMOVEMENTCOMPONENT_H

#include <engine/structure/Component.h>

class CameraMovementComponent : public Component {

    bool _w, _a, _s, _d, _shift, _space;
    float _speed;

    glm::vec2 _eulerAngles;

public:

    CameraMovementComponent();

    void onKey(const KeyboardEvent& event) override;

    void onCursorMove(const CursorMoveEvent& event) override;

    void onUpdate(float deltaTime) override;

    float getSpeed() const;

    void setSpeed(float speed);
};


#endif //VRTRACKING_CAMERAMOVEMENTCOMPONENT_H
