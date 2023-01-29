//
// Created by gaelr on 23/10/2022.
//

#include "Transform.h"

#include <glm/gtx/quaternion.hpp>

#include <engine/structure/GameObject.h>
#include <util/GLMUtils.h>

uint64_t TRANSFORM_ID_GENERATOR = 1;

Transform::Transform(IdentifiableWrapper<GameObject> object) :
        _id(TRANSFORM_ID_GENERATOR++),
        _position(),
        _rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
        _scale(1.0f, 1.0f, 1.0f),
        _gameObject(object),
        _parentIdOnLastRefresh(0),
        _dirty(false),
        _localModel(1.0f),
        _model(1.0f) {
}

IdentifiableWrapper<GameObject> Transform::getGameObject() const {
    return _gameObject;
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
    _rotation = glm::quatLookAt(glm::normalize(direction), glm::vec3(0, 1, 0));
    return _rotation;
}

const glm::quat& Transform::rotate(const glm::vec3& direction, float angle) {
    _dirty = true;
    _rotation = glm::angleAxis(angle, glm::normalize(direction)) * _rotation;
    return _rotation;
}

const glm::mat4& Transform::getModel() {
    recalculateIfRequired();
    return _model;
}

const glm::mat4& Transform::getNormal() {
    recalculateIfRequired();
    return _normal;
}

void Transform::recalculateIfRequired() {
    auto parent = _gameObject->getParent();
    if (_dirty) {
        _dirty = false;
        glm::trs(_localModel, _position, _rotation, _scale);
        glm::normal_matrix(_localNormal, _rotation, _scale);

        // Apply parent transformation
        if (parent != nullptr) {
            auto& t = parent->getTransform();
            _model = t.getModel() * _localModel;
            _normal = t.getNormal() * _localNormal;
            _parentIdOnLastRefresh = t._id;
        } else {
            _model = _localModel;
            _normal = _localNormal;
            _parentIdOnLastRefresh = 0;
        }
        _id = TRANSFORM_ID_GENERATOR++;
    } else {
        // Check if parent was modified.
        if (parent == nullptr && _parentIdOnLastRefresh != 0) {
            _model = _localModel;
            _normal = _localNormal;
            _parentIdOnLastRefresh = 0;
            _id = TRANSFORM_ID_GENERATOR++;
        } else if (parent != nullptr) {
            auto& t = parent->getTransform();
            if (_parentIdOnLastRefresh != t._id || t._dirty) {
                _model = t.getModel() * _localModel;
                _normal = t.getNormal() * _localNormal;
                _parentIdOnLastRefresh = t._id;
                _id = TRANSFORM_ID_GENERATOR++;
            }
        }
    }
}
