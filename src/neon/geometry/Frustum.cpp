//
// Created by grial on 19/10/22.
//

#include "Frustum.h"

namespace neon
{

    Frustum::Frustum(float near, float far, float aspectRatio, float fovYRadians) :
        _near(near),
        _far(far),
        _aspectRatio(aspectRatio),
        _fovYRadians(fovYRadians),
        _projection(rush::Mat4f::perspective<rush::Hand::Right, rush::ProjectionFormat::Vulkan>(fovYRadians,
                                                                                                aspectRatio, near, far))
    {
    }

    const rush::Mat4f& Frustum::getProjection() const
    {
        return _projection;
    }

    float Frustum::getNear() const
    {
        return _near;
    }

    float Frustum::getFar() const
    {
        return _far;
    }

    float Frustum::getAspectRatio() const
    {
        return _aspectRatio;
    }

    float Frustum::getFovYRadians() const
    {
        return _fovYRadians;
    }

    Frustum Frustum::withNear(float near) const
    {
        return {near, _far, _aspectRatio, _fovYRadians};
    }

    Frustum Frustum::withFar(float far) const
    {
        return {_near, far, _aspectRatio, _fovYRadians};
    }

    Frustum Frustum::withAspectRatio(float aspectRatio) const
    {
        return {_near, _far, aspectRatio, _fovYRadians};
    }

    Frustum Frustum::withFov(float fovYDegrees) const
    {
        return {_near, _far, _aspectRatio, fovYDegrees};
    }

    const rush::Mat4f& Frustum::getInverseProjection() const
    {
        if (!_inverseProjection.has_value()) {
            auto* f = const_cast<Frustum*>(this);
            f->_inverseProjection = _projection.inverse();
        }
        return _inverseProjection.value();
    }
} // namespace neon