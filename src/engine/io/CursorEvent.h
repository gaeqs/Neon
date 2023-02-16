//
// Created by grial on 31/10/22.
//

#ifndef VRTRACKING_CURSOREVENT_H
#define VRTRACKING_CURSOREVENT_H

#include <glm/glm.hpp>

namespace neon {

    struct CursorMoveEvent {
        glm::dvec2 position;
        glm::dvec2 delta;
    };
}

#endif //VRTRACKING_CURSOREVENT_H
