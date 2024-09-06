//
// Created by grial on 31/10/22.
//

#ifndef VRTRACKING_CURSOREVENT_H
#define VRTRACKING_CURSOREVENT_H

#include <rush/rush.h>

namespace neon {

    struct CursorMoveEvent {
        rush::Vec2f position;
        rush::Vec2f delta;
    };
}

#endif //VRTRACKING_CURSOREVENT_H
