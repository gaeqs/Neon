//
// Created by gaelr on 23/10/2022.
//

#include "Transform.h"

#include <glm/gtx/quaternion.hpp>

#include <engine/structure/GameObject.h>
#include "util/GLMUtils.h"

uint64_t TRANSFORM_ID_GENERATOR = 1;

Transform::Transform() :
        _id(TRANSFORM_ID_GENERATOR++),
        _position(),
        _rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
        _scale(1.0f, 1.0f, 1.0f),
        _parent(),
        _parentIdOnLastRefresh(0),
        _dirty(false),
        _localModel(1.0f),
        _model(1.0f) {
}

IdentifiableWrapper<GameObject> Transform::getParent() const {
    return _parent;
}

void Transform::setParent(const IdentifiableWrapper<GameObject>& parent) {
    _parent = parent;
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
        glm::trs(_localModel, _position, _rotation, _scale);

        // Apply parent transformation
        if (_parent != nullptr) {
            auto& t = _parent->getTransform();
            _model = t.getModel() * _localModel;
            _parentIdOnLastRefresh = t._id;
        } else {
            _model = _localModel;
            _parentIdOnLastRefresh = 0;
        }
        _id = TRANSFORM_ID_GENERATOR++;
    } else {
        // Check if parent was modified.
        if (_parent == nullptr && _parentIdOnLastRefresh != 0) {
            _model = _localModel;
            _parentIdOnLastRefresh = 0;
            _id = TRANSFORM_ID_GENERATOR++;
        } else if (_parent != nullptr) {
            auto& t = _parent->getTransform();
            if (_parentIdOnLastRefresh != t._id || t._dirty) {
                _model = t.getModel() * _localModel;
                _parentIdOnLastRefresh = t._id;
                _id = TRANSFORM_ID_GENERATOR++;
            }
        }
    }

    return _model;
}