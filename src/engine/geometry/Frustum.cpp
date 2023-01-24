//
// Created by grial on 19/10/22.
//

#include "Frustum.h"

#include <glm/gtc/matrix_transform.hpp>

Frustum::Frustum(float near, float far, float aspectRatio, float fovYDegrees) :
        _near(near),
        _far(far),
        _aspectRatio(aspectRatio),
        _fovYDegrees(fovYDegrees),
        _projection(glm::perspective(fovYDegrees, aspectRatio, near, far)) {
#ifdef USE_VULKAN
    _projection[1][1] *= -1;
#endif
}

const glm::mat4& Frustum::getProjection() const {
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

const glm::mat4& Frustum::getInverseProjection() const {
    if(!_inverseProjection.has_value()) {
        auto* f = const_cast<Frustum*>(this);
        f->_inverseProjection = glm::inverse(_projection);
    }
    return _inverseProjection.value();
}
