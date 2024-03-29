//
// Created by gaelr on 26/10/2022.
//

#include "CameraMovementComponent.h"

#include <engine/structure/Room.h>
#include <engine/geometry/Camera.h>
#include <engine/io/KeyboardEvent.h>
#include <engine/io/CursorEvent.h>

namespace neon {
    CameraMovementComponent::CameraMovementComponent() :
            _w(),
            _a(),
            _s(),
            _d(),
            _shift(),
            _space(),
            _speed(1.0f),
            _eulerAngles() {

    }

    void CameraMovementComponent::onKey(const KeyboardEvent& event) {
        rush::Vec3f movement;
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
            case KeyboardKey::E:
                _shift = event.action != KeyboardAction::RELEASE;
                break;
            case KeyboardKey::SPACE:
            case KeyboardKey::Q:
                _space = event.action != KeyboardAction::RELEASE;
                break;
            default:
                break;
        }
    }

    void CameraMovementComponent::onCursorMove(const CursorMoveEvent& event) {
        constexpr float LIMIT = 1.5707963f - 0.1f; // PI / 2 - 0.1

        auto& camera = getRoom()->getCamera();

        _eulerAngles -= event.delta.cast<float>()(1, 0).toVec() / 100.0f;
        _eulerAngles.x() = std::clamp(_eulerAngles.x(), -LIMIT, LIMIT);

        auto qX = rush::Quatf::angleAxis(_eulerAngles.x(), {1.0f, 0.0f, 0.0f});
        auto qY = rush::Quatf::angleAxis(_eulerAngles.y(), {0.0f, 1.0f, 0.0f});
        camera.setRotation(qY * qX);
    }

    void CameraMovementComponent::onUpdate(float deltaTime) {
        constexpr float EPSILON = 0.0001f;

        auto& camera = getGameObject()->getRoom()->getCamera();
        rush::Vec3f direction;

        rush::Vec3f forwardClamped = camera.getForward();
        forwardClamped.y() = 0.0f;
        forwardClamped = forwardClamped.normalized();

        if (_w) direction += forwardClamped;
        if (_s) direction -= forwardClamped;
        if (_d) direction += camera.getRight();
        if (_a) direction -= camera.getRight();
        if (_space) direction += rush::Vec3f(0.0f, 1.0f, 0.0f);
        if (_shift) direction -= rush::Vec3f(0.0f, 1.0f, 0.0f);

        if (std::abs(direction.x()) < EPSILON
            && std::abs(direction.y()) < EPSILON
            && std::abs(direction.z()) < EPSILON)
            return;

        auto offset = direction.normalized() * (_speed * deltaTime);
        camera.move(offset);
    }

    float CameraMovementComponent::getSpeed() const {
        return _speed;
    }

    void CameraMovementComponent::setSpeed(float speed) {
        _speed = speed;
    }
}
