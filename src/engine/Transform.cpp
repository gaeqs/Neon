//
// Created by gaelr on 23/10/2022.
//

#include "Transform.h"
#include <glm/gtx/quaternion.hpp>

Transform::Transform() :
        _position(),
        _rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
        _scale(1.0f, 1.0f, 1.0f),
        _dirty(false),
        _model(1.0f) {
}

const glm::vec3& Transform::getPosition() const {
    return _position;
}

const glm::quat& Transform::getRotation() const {
    return _rotation;
}

const glm::vec3& Transform::getScale() const {
    return _scale;
}

void Transform::setPosition(const glm::vec3& position) {
    _dirty = true;
    _position = position;
}

void Transform::setRotation(const glm::quat& rotation) {
    _dirty = true;
    _rotation = rotation;
}

void Transform::setScale(const glm::vec3& scale) {
    _dirty = true;
    _scale = scale;
}


const glm::vec3& Transform::move(const glm::vec3& offset) {
    _dirty = true;
    _position += offset;
    return _position;
}

const glm::quat& Transform::lookAt(const glm::vec3& direction) {
    _dirty = true;
    _rotation = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
    return _rotation;
}

const glm::quat& Transform::rotate(const glm::vec3& direction, float angle) {
    _dirty = true;
    _rotation = glm::angleAxis(angle, direction) * _rotation;
    return _rotation;
}

const glm::mat4& Transform::getModel() {
    if (_dirty) {
        _dirty = false;
        _model = glm::toMat4(_rotation) * glm::scale(glm::mat4(1.0f), _scale);
        _model = glm::translate(_model, _position);
    }

    return _model;
}
