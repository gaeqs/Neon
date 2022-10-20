//
// Created by grial on 19/10/22.
//

#include "Camera.h"


void Camera::recalculateViewMatrix() {
    _dirtyView = false;
    _view = glm::lookAt(_position, _position + getForward(), getUp());
}

Camera::Camera(const Frustum& frustum) :
        _position(),
        _rotation(),
        _rotationInverse(),
        _frustum(frustum),
        _dirtyView(false),
        _dirtyProjection(true),
        _view(1.0f),
        _viewProjection(1.0f) {
}

const glm::vec3& Camera::getPosition() const {
    return _position;
}

const glm::quat& Camera::getRotation() const {
    return _rotation;
}

const Frustum& Camera::getFrustum() const {
    return _frustum;
}

glm::vec3 Camera::getForward() const {
    return _rotationInverse * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Camera::getUp() const {
    return _rotationInverse * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Camera::getRight() const {
    return _rotationInverse * glm::vec3(1.0f, 0.0f, 0.0f);
}

void Camera::setPosition(const glm::vec3& position) {
    _dirtyView = true;
    _position = position;
}

void Camera::setRotation(const glm::quat& rotation) {
    _dirtyView = true;
    _rotation = rotation;
    _rotationInverse = glm::inverse(_rotation);
}

void Camera::setFrustum(const Frustum& frustum) {
    _dirtyProjection = true;
    _frustum = frustum;
}

const glm::vec3& Camera::move(const glm::vec3& offset) {
    _dirtyView = true;
    _position += offset;
    return _position;
}

const glm::quat& Camera::lookAt(const glm::vec3& direction) {
    _dirtyView = true;
    _rotation = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
    _rotationInverse = glm::inverse(_rotation);
    return _rotation;
}

const glm::quat& Camera::rotate(const glm::vec3& direction, float angle) {
    _dirtyView = true;
    _rotation = glm::angleAxis(angle, direction) * _rotation;
    _rotationInverse = glm::inverse(_rotation);
    return _rotation;
}

const glm::mat4& Camera::getView() {
    if (_dirtyView) {
        _dirtyProjection = true;
        recalculateViewMatrix();
    }

    return _view;
}

const glm::mat4& Camera::getViewProjection() {
    if (!_dirtyView && !_dirtyProjection) return _viewProjection;
    if (_dirtyView) {
        recalculateViewMatrix();
    }

    _dirtyProjection = false;
    _viewProjection = _frustum.getProjection() * _view;
    return _viewProjection;
}
