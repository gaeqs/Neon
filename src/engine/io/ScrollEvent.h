//
// Created by grial on 31/10/22.
//

#ifndef NEON_SCROLLEVENT_H
#define NEON_SCROLLEVENT_H

#include <glm/glm.hpp>

namespace neon {

    struct ScrollEvent {
        glm::dvec2 delta;
    };
}

#endif //NEON_SCROLLEVENT_H
