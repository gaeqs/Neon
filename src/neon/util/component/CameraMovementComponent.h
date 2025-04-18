//
// Created by gaelr on 26/10/2022.
//

#ifndef NEON_CAMERAMOVEMENTCOMPONENT_H
#define NEON_CAMERAMOVEMENTCOMPONENT_H

#include <rush/rush.h>

#include <neon/structure/Component.h>

namespace neon
{
    class CameraMovementComponent : public Component
    {
        bool _w, _a, _s, _d, _shift, _space;
        float _speed;

        rush::Vec2f _eulerAngles;
        rush::Vec2f _toEulerAngles;

        void updateRotation(float deltaTime);

        void updatePosition(float deltaTime);

      public:
        CameraMovementComponent();

        void onKey(const KeyboardEvent& event) override;

        void onCursorMove(const CursorMoveEvent& event) override;

        void onUpdate(float deltaTime) override;

        [[nodiscard]] float getSpeed() const;

        void setSpeed(float speed);
    };

    REGISTER_COMPONENT(CameraMovementComponent, "Camera Move")
} // namespace neon

#endif //NEON_CAMERAMOVEMENTCOMPONENT_H
