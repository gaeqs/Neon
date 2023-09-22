//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_FRUSTUM_H
#define RVTRACKING_FRUSTUM_H

#include <optional>

#include <rush/rush.h>

namespace neon {

    class Frustum {

        float _near;
        float _far;
        float _aspectRatio;
        float _fovYDegrees;

        rush::Mat4f _projection;
        std::optional<rush::Mat4f> _inverseProjection;

    public:

        Frustum(float near, float far, float aspectRatio, float fovYDegrees);

        float getNear() const;

        float getFar() const;

        float getAspectRatio() const;

        float getFovYDegrees() const;

        const rush::Mat4f& getProjection() const;

        const rush::Mat4f& getInverseProjection() const;

        Frustum withNear(float near) const;

        Frustum withFar(float far) const;

        Frustum withAspectRatio(float aspectRatio) const;

        Frustum withFov(float fovYDegrees) const;
    };
}

#endif //RVTRACKING_FRUSTUM_H
