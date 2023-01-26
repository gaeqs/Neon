//
// Created by gaelr on 20/11/2022.
//

#include "ConstantRotationComponent.h"

void ConstantRotationComponent::onStart() {
    _speed = static_cast<float>(std::rand())
            / static_cast<float>(RAND_MAX) * 5;
}

void ConstantRotationComponent::onUpdate(float deltaTime) {
    getGameObject()->getTransform().rotate(
            glm::vec3(0, 1, 0),
            deltaTime * _speed / 10.0f
    );
}
