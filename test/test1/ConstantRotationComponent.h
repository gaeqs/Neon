//
// Created by gaelr on 20/11/2022.
//

#ifndef NEON_CONSTANTROTATIONCOMPONENT_H
#define NEON_CONSTANTROTATIONCOMPONENT_H

#include <engine/Engine.h>

class ConstantRotationComponent : public neon::Component {

    float _speed;
    glm::vec3 _axis;

public:

    ConstantRotationComponent();

    [[nodiscard]] float getSpeed() const;

    void setSpeed(float speed);

    [[nodiscard]] const glm::vec3& getAxis() const;

    void setAxis(const glm::vec3& axis);

    void onUpdate(float deltaTime) override;

    void drawEditor() override;


};
REGISTER_COMPONENT(ConstantRotationComponent, "Constant Rotation")


#endif //NEON_CONSTANTROTATIONCOMPONENT_H
