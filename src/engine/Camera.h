//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_CAMERA_H
#define RVTRACKING_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <engine/Frustum.h>

class Camera {

    glm::vec3 _position;
    glm::quat _rotation;
    glm::quat _rotationInverse;
    Frustum _frustum;

    bool _dirtyView;
    bool _dirtyProjection;

    glm::mat4 _view;
    glm::mat4 _viewProjection;

    void recalculateViewMatrix();

public:

    Camera(const Camera& other) = delete;

    Camera(const Frustum& frustum);

    const glm::vec3& getPosition() const;

    const glm::quat& getRotation() const;

    const Frustum& getFrustum() const;

    glm::vec3 getForward() const;

    glm::vec3 getUp() const;

    glm::vec3 getRight() const;

    void setPosition(const glm::vec3& position);

    void setRotation(const glm::quat& rotation);

    void setFrustum(const Frustum& frustum);

    const glm::vec3& move(const glm::vec3& offset);

    const glm::quat& lookAt(const glm::vec3& direction);

    const glm::quat& rotate(const glm::vec3& direction, float angle);

    const glm::quat& rotate(const glm::quat& quaternion);

    const glm::mat4& getView();

    const glm::mat4& getViewProjection();
};


#endif //RVTRACKING_CAMERA_H
