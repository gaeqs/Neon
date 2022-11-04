//
// Created by gaelr on 26/10/2022.
//

#include "CameraMovementComponent.h"

#include "engine/Room.h"
#include "engine/Camera.h"
#include "engine/KeyboardEvent.h"
#include "engine/CursorEvent.h"

#include <glm/glm.hpp>

CameraMovementComponent::CameraMovementComponent() :
        _w(),
        _a(),
        _s(),
        _d(),
        _shift(),
        _space(),
        _speed(1.0f),
        _eulerAngles(0.0f) {

}

void CameraMovementComponent::onKey(const KeyboardEvent& event) {
    glm::vec3 movement;
    switch (event.key) {
        case KeyboardKey::W:
            _w = event.action != KeyboardAction::RELEASE;
            break;
        case KeyboardKey::S:
            _s = event.action != KeyboardAction::RELEASE;
            break;
        case KeyboardKey::A:
            _a = event.action != KeyboardAction::RELEASE;
            break;
        case KeyboardKey::D:
            _d = event.action != KeyboardAction::RELEASE;
            break;
        case KeyboardKey::LEFT_SHIFT:
            _shift = event.action != KeyboardAction::RELEASE;
            break;
        case KeyboardKey::SPACE:
            _space = event.action != KeyboardAction::RELEASE;
            break;
        default:
            break;
    }
}

void CameraMovementComponent::onCursorMove(const CursorMoveEvent& event) {
    constexpr float LIMIT = 1.5707963f - 0.1f; // PI / 2 - 0.1

    auto& camera = getRoom()->getCamera();

    _eulerAngles.x += static_cast<float>(event.delta.y / 100.0);
    _eulerAngles.y += static_cast<float>(event.delta.x / 100.0);


    _eulerAngles.x = glm::clamp(_eulerAngles.x, -LIMIT, LIMIT);

    camera.setRotation(
            glm::angleAxis(_eulerAngles.x, glm::vec3(1, 0, 0)) *
            glm::angleAxis(_eulerAngles.y, glm::vec3(0, 1, 0))
    );
}

void CameraMovementComponent::onUpdate(float deltaTime) {
    auto& camera = getGameObject()->getRoom()->getCamera();
    glm::vec3 direction(0.0f);

    glm::vec3 forwardClamped = camera.getForward();
    forwardClamped.y = 0.0f;
    forwardClamped = glm::normalize(forwardClamped);

    if (_w) direction += forwardClamped;
    if (_s) direction -= forwardClamped;
    if (_d) direction += camera.getRight();
    if (_a) direction -= camera.getRight();
    if (_space) direction += glm::vec3(0.0f, 1.0f, 0.0f);
    if (_shift) direction -= glm::vec3(0.0f, 1.0f, 0.0f);

    if (std::abs(direction.x) < FLT_EPSILON
        && std::abs(direction.y) < FLT_EPSILON
        && std::abs(direction.z) < FLT_EPSILON)
        return;

    auto offset = glm::normalize(direction) * (_speed * deltaTime);
    camera.move(offset);
}

float CameraMovementComponent::getSpeed() const {
    return _speed;
}

void CameraMovementComponent::setSpeed(float speed) {
    _speed = speed;
}
