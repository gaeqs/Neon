//
// Created by gaelr on 24/10/2022.
//

#ifndef RVTRACKING_TESTVERTEX_H
#define RVTRACKING_TESTVERTEX_H

#include <engine/model/InputDescription.h>

struct TestVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    static InputDescription getDescription() {
        InputDescription description(
                sizeof(TestVertex),
                InputRate::VERTEX
        );
        description.addAttribute(3, 0);
        description.addAttribute(3, sizeof(float) * 3);
        description.addAttribute(2, sizeof(float) * 6);

        return description;
    }


#ifdef USE_OPENGL
#include <glm/glm.hpp>
#include <glad/glad.h>
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
#endif

#ifdef USE_VULKAN

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(TestVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescriptions() {

        VkVertexInputAttributeDescription positionDescription{
                0, 0, VK_FORMAT_R32G32B32_SFLOAT,
                static_cast<uint32_t>(offsetof(TestVertex, position))};
        VkVertexInputAttributeDescription normalDescription{
                1, 0, VK_FORMAT_R32G32B32_SFLOAT,
                static_cast<uint32_t>(offsetof(TestVertex, normal))};
        VkVertexInputAttributeDescription texCoordsDescription{
                2, 0, VK_FORMAT_R32G32_SFLOAT,
                static_cast<uint32_t>(offsetof(TestVertex, texCoords))};

        return {positionDescription, normalDescription, texCoordsDescription};
    }

#endif

    static TestVertex fromAssimp(
            const glm::vec3& position,
            const glm::vec3& normal,
            const glm::vec4& color,
            const glm::vec2& texCoords) {
        return {position, normal, texCoords};
    }
};


#endif //RVTRACKING_TESTVERTEX_H
