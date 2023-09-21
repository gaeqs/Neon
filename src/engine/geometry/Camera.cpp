//
// Created by grial on 19/10/22.
//

#include "Camera.h"

namespace neon {

    void Camera::recalculateViewMatrix() {
        _dirtyView = false;
        _view = rush::Mat4f::lookAt(_position, getForward(), getUp());
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

    const rush::Vec3f& Camera::getPosition() const {
        return _position;
    }

    const rush::Quatf& Camera::getRotation() const {
        return _rotation;
    }

    const Frustum& Camera::getFrustum() const {
        return _frustum;
    }

    rush::Vec3f Camera::getForward() const {
        return _rotationInverse * rush::Vec3f(0.0f, 0.0f, -1.0f);
    }

    rush::Vec3f Camera::getUp() const {
        return _rotationInverse * rush::Vec3f(0.0f, 1.0f, 0.0f);
    }

    rush::Vec3f Camera::getRight() const {
        return _rotationInverse * rush::Vec3f(1.0f, 0.0f, 0.0f);
    }

    void Camera::setPosition(const rush::Vec3f& position) {
        _dirtyView = true;
        _position = position;
    }

    void Camera::setRotation(const rush::Quatf& rotation) {
        _dirtyView = true;
        _rotation = rotation;
        _rotationInverse = _rotation.inverse();
    }

    void Camera::setFrustum(const Frustum& frustum) {
        _dirtyProjection = true;
        _frustum = frustum;
    }

    const rush::Vec3f& Camera::move(const rush::Vec3f& offset) {
        _dirtyView = true;
        _position += offset;
        return _position;
    }

    const rush::Quatf& Camera::lookAt(const rush::Vec3f& direction) {
        _dirtyView = true;
        _rotation = rush::Quatf::lookAt(direction.normalized());
        _rotationInverse = _rotation.inverse();
        return _rotation;
    }

    const rush::Quatf&
    Camera::rotate(const rush::Vec3f& direction, float angle) {
        _dirtyView = true;
        _rotation = rush::Quatf::angleAxis(angle, direction) * _rotation;
        _rotationInverse = _rotation.inverse();
        return _rotation;
    }

    const rush::Quatf& Camera::rotate(const rush::Quatf& quaternion) {
        _dirtyView = true;
        _rotation = quaternion * _rotation;
        _rotationInverse = _rotation.inverse();
        return _rotation;
    }

    const rush::Mat4f& Camera::getView() {
        if (_dirtyView) {
            _dirtyProjection = true;
            recalculateViewMatrix();
        }

        return _view;
    }

    const rush::Mat4f& Camera::getViewProjection() {
        if (!_dirtyView && !_dirtyProjection) return _viewProjection;
        if (_dirtyView) {
            recalculateViewMatrix();
        }

        _dirtyProjection = false;
        _viewProjection = _frustum.getProjection() * _view;
        return _viewProjection;
    }
}