//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_TRANSFORM_H
#define RVTRACKING_TRANSFORM_H


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {

    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;


    bool _dirty;
    glm::mat4 _model;

public:

    Transform();

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

};


#endif //RVTRACKING_TRANSFORM_H
