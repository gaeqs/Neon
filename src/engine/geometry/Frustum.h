//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_FRUSTUM_H
#define RVTRACKING_FRUSTUM_H

#include <optional>

#include <glm/glm.hpp>

class Frustum {

    float _near;
    float _far;
    float _aspectRatio;
    float _fovYDegrees;

    glm::mat4 _projection;
    std::optional<glm::mat4> _inverseProjection;

public:

    Frustum(float near, float far, float aspectRatio, float fovYDegrees);

    float getNear() const;

    float getFar() const;

    float getAspectRatio() const;

    float getFovYDegrees() const;

    const glm::mat4& getProjection() const;

    const glm::mat4& getInverseProjection() const;

    Frustum withNear(float near) const;

    Frustum withFar(float far) const;

    Frustum withAspectRatio(float aspectRatio) const;

    Frustum withFov(float fovYDegrees) const;
};


#endif //RVTRACKING_FRUSTUM_H
