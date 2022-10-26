//
// Created by gaelr on 26/10/2022.
//

#include "CameraMovementComponent.h"

#include <engine/Room.h>
#include <engine/Camera.h>

#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

CameraMovementComponent::CameraMovementComponent() :
        _w(),
        _a(),
        _s(),
        _d(),
        _shift(),
        _space(),
        _speed(1.0f) {

}

void CameraMovementComponent::onKey(int32_t key, int32_t scancode,
                                    int32_t action, int32_t mods) {
    glm::vec3 movement;
    switch (key) {
        case GLFW_KEY_W:
            _w = action != GLFW_RELEASE;
            break;
        case GLFW_KEY_S:
            _s = action != GLFW_RELEASE;
            break;
        case GLFW_KEY_A:
            _a = action != GLFW_RELEASE;
            break;
        case GLFW_KEY_D:
            _d = action != GLFW_RELEASE;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            _shift = action != GLFW_RELEASE;
            break;
        case GLFW_KEY_SPACE:
            _space = action != GLFW_RELEASE;
            break;
        default:
            break;
    }
}

void CameraMovementComponent::onUpdate(float deltaTime) {
    auto& camera = getGameObject()->getRoom()->getCamera();
    glm::vec3 direction(0.0f);
    if (_w) direction += camera.getForward();
    if (_s) direction -= camera.getForward();
    if (_d) direction += camera.getRight();
    if (_a) direction -= camera.getRight();
    if (_space) direction += camera.getUp();
    if (_shift) direction -= camera.getUp();

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
