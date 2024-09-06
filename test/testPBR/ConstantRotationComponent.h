//
// Created by gaelr on 20/11/2022.
//

#ifndef NEON_CONSTANTROTATIONCOMPONENT_H
#define NEON_CONSTANTROTATIONCOMPONENT_H

#include <neon/Neon.h>

class ConstantRotationComponent : public neon::Component {

    float _speed;
    rush::Vec3f _axis;

public:

    ConstantRotationComponent();

    [[nodiscard]] float getSpeed() const;

    void setSpeed(float speed);

    [[nodiscard]] const rush::Vec3f& getAxis() const;

    void setAxis(const rush::Vec3f& axis);

    void onUpdate(float deltaTime) override;

    void drawEditor() override;


};
REGISTER_COMPONENT(ConstantRotationComponent, "Constant Rotation")


#endif //NEON_CONSTANTROTATIONCOMPONENT_H
