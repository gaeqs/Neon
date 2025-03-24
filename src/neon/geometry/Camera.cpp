//
// Created by grial on 19/10/22.
//

#include "Camera.h"

namespace neon
{
    void Camera::recalculateViewMatrix()
    {
        _dirtyView = false;
        _view = rush::Mat4f::lookAt(_position, getForward(), getUp());
    }

    Camera::Camera(const Frustum& frustum) :
        _frustum(frustum),
        _dirtyView(false),
        _dirtyProjection(true),
        _view(1.0f),
        _viewProjection(1.0f)
    {
    }

    const rush::Vec3f& Camera::getPosition() const
    {
        return _position;
    }

    const rush::Quatf& Camera::getRotation() const
    {
        return _rotation;
    }

    const Frustum& Camera::getFrustum() const
    {
        return _frustum;
    }

    rush::Vec3f Camera::getForward() const
    {
        return _rotation * rush::Vec3f(0.0f, 0.0f, -1.0f);
    }

    rush::Vec3f Camera::getUp() const
    {
        return _rotation * rush::Vec3f(0.0f, 1.0f, 0.0f);
    }

    rush::Vec3f Camera::getRight() const
    {
        return _rotation * rush::Vec3f(1.0f, 0.0f, 0.0f);
    }

    void Camera::setPosition(const rush::Vec3f& position)
    {
        _dirtyView = true;
        _position = position;
    }

    void Camera::setRotation(const rush::Quatf& rotation)
    {
        _dirtyView = true;
        _rotation = rotation;
        _rotationInverse = _rotation.inverse();

#ifndef NDEBUG
        if (!_rotation.isNormalized()) {
            throw std::runtime_error("Rotation is not normalized ");
        }
#endif
    }

    void Camera::setFrustum(const Frustum& frustum)
    {
        _dirtyProjection = true;
        _frustum = frustum;
    }

    const rush::Vec3f& Camera::move(const rush::Vec3f& offset)
    {
        _dirtyView = true;
        _position += offset;
        return _position;
    }

    const rush::Quatf& Camera::lookAt(const rush::Vec3f& direction)
    {
        _dirtyView = true;
        _rotation = rush::Quatf::lookAt(direction.normalized());
        _rotationInverse = _rotation.inverse();

#ifndef NDEBUG
        if (!_rotation.isNormalized()) {
            throw std::runtime_error("Rotation is not normalized ");
        }
#endif

        return _rotation;
    }

    const rush::Quatf& Camera::rotate(const rush::Vec3f& direction, float angle)
    {
        _dirtyView = true;
        _rotation = rush::Quatf::angleAxis(angle, direction) * _rotation;

#ifndef NDEBUG
        if (!_rotation.isNormalized()) {
            throw std::runtime_error("Rotation is not normalized ");
        }
#endif

        _rotationInverse = _rotation.inverse();
        return _rotation;
    }

    const rush::Quatf& Camera::rotate(const rush::Quatf& quaternion)
    {
        _dirtyView = true;
        _rotation = quaternion * _rotation;
        _rotationInverse = _rotation.inverse();

#ifndef NDEBUG
        if (!_rotation.isNormalized()) {
            throw std::runtime_error("Rotation is not normalized ");
        }
#endif

        return _rotation;
    }

    const rush::Mat4f& Camera::getView()
    {
        if (_dirtyView) {
            _dirtyProjection = true;
            recalculateViewMatrix();
        }

        return _view;
    }

    const rush::Mat4f& Camera::getViewProjection()
    {
        if (!_dirtyView && !_dirtyProjection) {
            return _viewProjection;
        }
        if (_dirtyView) {
            recalculateViewMatrix();
        }

        _dirtyProjection = false;
        _viewProjection = _frustum.getProjection() * _view;
        return _viewProjection;
    }

    std::array<rush::Plane<float>, 6> Camera::getPlanes()
    {
        auto& vp = getViewProjection();
        rush::Vec3f r0 = vp.row(0);
        rush::Vec3f r1 = vp.row(1);
        rush::Vec3f r2 = vp.row(2);
        rush::Vec3f r3 = vp.row(3);

        return {
            // Left Plane: r3 + r0
            rush::Plane<float>(r3 + r0, vp(3, 3) + vp(3, 0)).normalized(),
            // Right Plane: r3 - r0
            rush::Plane<float>(r3 - r0, vp(3, 3) - vp(3, 0)).normalized(),
            // Bottom Plane: r3 + r1
            rush::Plane<float>(r3 + r1, vp(3, 3) + vp(3, 1)).normalized(),
            // Top Plane: r3 - r1
            rush::Plane<float>(r3 - r1, vp(3, 3) - vp(3, 1)).normalized(),
            // Near Plane: r3 + r2
            rush::Plane<float>(r3 + r2, vp(3, 3) + vp(3, 2)).normalized(),
            // Far Plane: r3 - r2
            rush::Plane<float>(r3 - r2, vp(3, 3) - vp(3, 2)).normalized(),
        };
    }
} // namespace neon
