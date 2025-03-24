//
// Created by gaelr on 23/10/2022.
//

#include "Transform.h"

#include <neon/structure/GameObject.h>

namespace neon
{

    uint64_t TRANSFORM_ID_GENERATOR = 1;

    Transform::Transform(IdentifiableWrapper<GameObject> object) :
        _id(TRANSFORM_ID_GENERATOR++),
        _position(),
        _rotation(rush::Quatf::euler({0.0f, 0.0f, 0.0f})),
        _scale(1.0f, 1.0f, 1.0f),
        _gameObject(object),
        _parentIdOnLastRefresh(0),
        _dirty(false),
        _localModel(1.0f),
        _model(1.0f)
    {
    }

    IdentifiableWrapper<GameObject> Transform::getGameObject() const
    {
        return _gameObject;
    }

    const rush::Vec3f& Transform::getPosition() const
    {
        return _position;
    }

    const rush::Quatf& Transform::getRotation() const
    {
        return _rotation;
    }

    const rush::Vec3f& Transform::getScale() const
    {
        return _scale;
    }

    void Transform::setPosition(const rush::Vec3f& position)
    {
        _dirty = true;
        _position = position;
    }

    void Transform::setRotation(const rush::Quatf& rotation)
    {
        _dirty = true;
        _rotation = rotation;
    }

    void Transform::setScale(const rush::Vec3f& scale)
    {
        _dirty = true;
        _scale = scale;
    }

    const rush::Vec3f& Transform::move(const rush::Vec3f& offset)
    {
        _dirty = true;
        _position += offset;
        return _position;
    }

    const rush::Quatf& Transform::lookAt(const rush::Vec3f& direction)
    {
        _dirty = true;
        _rotation = rush::Quatf::lookAt(direction.normalized());
        return _rotation;
    }

    const rush::Quatf& Transform::rotate(const rush::Vec3f& direction, float angle)
    {
        _dirty = true;
        _rotation = rush::Quatf::angleAxis(angle, direction.normalized()) * _rotation;
        return _rotation;
    }

    const rush::Mat4f& Transform::getModel()
    {
        recalculateIfRequired();
        return _model;
    }

    const rush::Mat4f& Transform::getNormal()
    {
        recalculateIfRequired();
        return _normal;
    }

    void Transform::recalculateIfRequired()
    {
        auto parent = _gameObject->getParent();
        if (_dirty) {
            _dirty = false;
            _localModel = rush::Mat4f::model(_scale, _rotation, _position);
            _localNormal = rush::Mat4f::normal(_scale, _rotation);

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
} // namespace neon