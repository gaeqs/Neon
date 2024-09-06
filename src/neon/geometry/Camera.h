//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_CAMERA_H
#define RVTRACKING_CAMERA_H

#include <rush/rush.h>

#include <neon/geometry/Frustum.h>

namespace neon {

    class Camera {

        rush::Vec3f _position;
        rush::Quatf _rotation;
        rush::Quatf _rotationInverse;
        Frustum _frustum;

        bool _dirtyView;
        bool _dirtyProjection;

        rush::Mat4f _view;
        rush::Mat4f _viewProjection;

        void recalculateViewMatrix();

    public:

        Camera(const Camera& other) = delete;

        Camera(const Frustum& frustum);

        const rush::Vec3f& getPosition() const;

        const rush::Quatf& getRotation() const;

        const Frustum& getFrustum() const;

        rush::Vec3f getForward() const;

        rush::Vec3f getUp() const;

        rush::Vec3f getRight() const;

        void setPosition(const rush::Vec3f& position);

        void setRotation(const rush::Quatf& rotation);

        void setFrustum(const Frustum& frustum);

        const rush::Vec3f& move(const rush::Vec3f& offset);

        const rush::Quatf& lookAt(const rush::Vec3f& direction);

        const rush::Quatf& rotate(const rush::Vec3f& direction, float angle);

        const rush::Quatf& rotate(const rush::Quatf& quaternion);

        const rush::Mat4f& getView();

        const rush::Mat4f& getViewProjection();
    };
}


#endif //RVTRACKING_CAMERA_H
