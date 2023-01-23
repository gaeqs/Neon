//
// Created by gaelr on 23/01/2023.
//

#include "GLMUtils.h"

#include <glm/gtc/type_ptr.hpp>


namespace glm {

    void trs(glm::mat4& result,
             const glm::vec3& t,
             const glm::quat& r,
             const glm::vec3& s) {
        float* p = glm::value_ptr(result);
        *p = (1.0f - 2.0f * (r.y * r.y + r.z * r.z)) * s.x;
        *++p = (r.x * r.y + r.z * r.w) * s.x * 2.0f;
        *++p = (r.x * r.z - r.y * r.w) * s.x * 2.0f;
        *++p = 0.0f;
        *++p = (r.x * r.y - r.z * r.w) * s.y * 2.0f;
        *++p = (1.0f - 2.0f * (r.x * r.x + r.z * r.z)) * s.y;
        *++p = (r.y * r.z + r.x * r.w) * s.y * 2.0f;
        *++p = 0.0f;
        *++p = (r.x * r.z + r.y * r.w) * s.z * 2.0f;
        *++p = (r.y * r.z - r.x * r.w) * s.z * 2.0f;
        *++p = (1.0f - 2.0f * (r.x * r.x + r.y * r.y)) * s.z;
        *++p = 0.0f;
        *++p = t.x;
        *++p = t.y;
        *++p = t.z;
        *++p = 1.0f;
    }
}