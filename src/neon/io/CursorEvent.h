//
// Created by grial on 31/10/22.
//

#ifndef NEON_CURSOREVENT_H
#define NEON_CURSOREVENT_H

#include <rush/rush.h>

namespace neon
{

    struct CursorMoveEvent
    {
        rush::Vec2f position;
        rush::Vec2f delta;
    };
} // namespace neon

#endif //NEON_CURSOREVENT_H
