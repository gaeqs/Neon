//
// Created by gaelr on 20/11/2022.
//

#include "ConstantRotationComponent.h"

void ConstantRotationComponent::onUpdate(float deltaTime) {
    getGameObject()->getTransform().rotate(
            glm::vec3(0, 1, 0),
            deltaTime / 10.0f
    );
}
