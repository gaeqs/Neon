//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_DEFAULTINSTANCINGDATA_H
#define NEON_DEFAULTINSTANCINGDATA_H

#include <glm/glm.hpp>
#include <engine/model/InputDescription.h>

struct DefaultInstancingData {
    glm::mat4 model;

    static InputDescription getInstancingDescription() {
        InputDescription description(
                sizeof(DefaultInstancingData),
                InputRate::INSTANCE
        );
        description.addAttribute(16, 0);

        return description;
    }

};

#endif //NEON_DEFAULTINSTANCINGDATA_H
