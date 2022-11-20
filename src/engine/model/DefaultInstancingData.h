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
#ifdef USE_VULKAN

    static VkVertexInputBindingDescription getInstancingBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 1;
        bindingDescription.stride = sizeof(DefaultInstancingData);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription>
    getInstancingAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> values;
        values.reserve(4);

        for (uint32_t i = 0; i < 4; ++i) {
            values.push_back(VkVertexInputAttributeDescription{
                    i + 3, 1, VK_FORMAT_R32G32B32A32_SFLOAT,
                    static_cast<uint32_t>(sizeof(float) * 4 * i)}
            );
        }
        return values;
    }

#endif

};

#endif //NEON_DEFAULTINSTANCINGDATA_H
