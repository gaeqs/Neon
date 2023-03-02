//
// Created by gaelr on 24/10/2022.
//

#ifndef RVTRACKING_TESTVERTEX_H
#define RVTRACKING_TESTVERTEX_H

#include <engine/model/InputDescription.h>

#include <glm/glm.hpp>

struct TestVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texCoords;

    static neon::InputDescription getDescription() {
        neon::InputDescription description(
                sizeof(TestVertex),
                neon::InputRate::VERTEX
        );
        description.addAttribute(3, 0);
        description.addAttribute(3, sizeof(float) * 3);
        description.addAttribute(3, sizeof(float) * 6);
        description.addAttribute(2, sizeof(float) * 9);

        return description;
    }

    static TestVertex fromAssimp(
            const glm::vec3& position,
            const glm::vec3& normal,
            const glm::vec3& tangent,
            const glm::vec4& color,
            const glm::vec2& texCoords) {
        return {position, normal, tangent, texCoords};
    }
};


#endif //RVTRACKING_TESTVERTEX_H
