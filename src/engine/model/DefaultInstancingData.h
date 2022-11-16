//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_DEFAULTINSTANCINGDATA_H
#define NEON_DEFAULTINSTANCINGDATA_H

#include <glm/glm.hpp>

struct DefaultInstancingData {
    glm::mat4 model;

#ifdef USE_OPENGL
    static void setupInstancingVAO(uint32_t first) {
        for (int i = 0; i < 4; ++i) {
            int id = i + first;
            glVertexAttribPointer(id, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(DefaultInstancingData),
                                  (void*) (sizeof(float) * 4 * i));
            glEnableVertexAttribArray(id);
            glVertexAttribDivisor(id, 1);
        }
    }
#endif

};

#endif //NEON_DEFAULTINSTANCINGDATA_H
