//
// Created by grial on 19/10/22.
//

#include "Frustum.h"


namespace neon {

    Frustum::Frustum(float near, float far, float aspectRatio,
                     float fovYDegrees) :
            _near(near),
            _far(far),
            _aspectRatio(aspectRatio),
            _fovYDegrees(fovYDegrees),
            _projection(rush::Mat4f::perspective(fovYDegrees, aspectRatio,
                                                 near, far)) {
#ifdef USE_VULKAN
        _projection[1][1] *= -1;
#endif
    }

    const rush::Mat4f& Frustum::getProjection() const {
        return _projection;
    }

    float Frustum::getNear() const {
        return _near;
    }

    float Frustum::getFar() const {
        return _far;
    }

    float Frustum::getAspectRatio() const {
        return _aspectRatio;
    }

    float Frustum::getFovYDegrees() const {
        return _fovYDegrees;
    }

    Frustum Frustum::withNear(float near) const {
        return {near, _far, _aspectRatio, _fovYDegrees};
    }

    Frustum Frustum::withFar(float far) const {
        return {_near, far, _aspectRatio, _fovYDegrees};
    }

    Frustum Frustum::withAspectRatio(float aspectRatio) const {
        return {_near, _far, aspectRatio, _fovYDegrees};
    }

    Frustum Frustum::withFov(float fovYDegrees) const {
        return {_near, _far, _aspectRatio, fovYDegrees};
    }

    const rush::Mat4f& Frustum::getInverseProjection() const {
        if (!_inverseProjection.has_value()) {
            auto* f = const_cast<Frustum*>(this);
            f->_inverseProjection = _projection.inverse();
        }
        return _inverseProjection.value();
    }
}