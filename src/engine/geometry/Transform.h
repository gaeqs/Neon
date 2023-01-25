//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_TRANSFORM_H
#define RVTRACKING_TRANSFORM_H


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <engine/structure/IdentifiableWrapper.h>

class GameObject;

class Transform {

    uint64_t _id;

    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;

    IdentifiableWrapper<GameObject> _parent;
    uint64_t _parentIdOnLastRefresh;

    bool _dirty;
    glm::mat4 _localModel;
    glm::mat4 _localNormal;
    glm::mat4 _model;
    glm::mat4 _normal;

    void recalculateIfRequired();

public:

    Transform();

    IdentifiableWrapper<GameObject> getParent() const;

    void setParent(const IdentifiableWrapper<GameObject>& parent);

    const glm::vec3& getPosition() const;

    const glm::quat& getRotation() const;

    const glm::vec3& getScale() const;

    void setPosition(const glm::vec3& position);

    void setRotation(const glm::quat& rotation);

    void setScale(const glm::vec3& scale);

    const glm::vec3& move(const glm::vec3& offset);

    const glm::quat& lookAt(const glm::vec3& direction);

    const glm::quat& rotate(const glm::vec3& direction, float angle);

    const glm::mat4& getModel();

    const glm::mat4& getNormal();

};


#endif //RVTRACKING_TRANSFORM_H
