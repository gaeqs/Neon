//
// Created by gaelr on 23/01/2023.
//

#ifndef NEON_GLMUTILS_H
#define NEON_GLMUTILS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace glm {

    void trs(glm::mat4& result,
             const glm::vec3& t,
             const glm::quat& r,
             const glm::vec3& s);

    void normal_matrix(glm::mat4& result,
             const glm::quat& r,
             const glm::vec3& s);

}


#endif //NEON_GLMUTILS_H
