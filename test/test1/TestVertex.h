//
// Created by gaelr on 24/10/2022.
//

#ifndef RVTRACKING_TESTVERTEX_H
#define RVTRACKING_TESTVERTEX_H

#include <glm/glm.hpp>
#include <glad/glad.h>

struct TestVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    static uint32_t setupVAO() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TestVertex),
                              (void*) 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TestVertex),
                              (void*) (3 * sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TestVertex),
                              (void*) (6 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        return 3;
    }

    static TestVertex fromAssimp(
            const glm::vec3& position,
            const glm::vec3& normal,
            const glm::vec4& color,
            const glm::vec2& texCoords) {
        return {position, normal, texCoords};
    }
};


#endif //RVTRACKING_TESTVERTEX_H
