//
// Created by gaelr on 5/07/23.
//

#ifndef NEON_MOUSEBUTTONEVENT_H
#define NEON_MOUSEBUTTONEVENT_H

#include "KeyboardEvent.h"

namespace neon
{

    enum class MouseButton
    {
        BUTTON_PRIMARY = 0,
        BUTTON_SECONDARY = 1,
        BUTTON_MIDDLE = 2,
        BUTTON_3 = 3,
        BUTTON_4 = 4,
        BUTTON_5 = 5,
        BUTTON_6 = 6,
        BUTTON_7 = 7,
        UNDEFINED = -1,
    };

    struct MouseButtonEvent
    {
        MouseButton button;
        KeyboardAction action;
        int modifiers;

        [[nodiscard]] inline bool isModifierActive(KeyboardModifier modifier) const
        {
            return (modifiers & static_cast<int>(modifier)) > 0;
        }
    };

} // namespace neon

#endif //NEON_MOUSEBUTTONEVENT_H
