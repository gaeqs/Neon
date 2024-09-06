//
// Copyright (c) 2015-2023 VG-Lab/URJC.
//
// Authors: Gael Rial Costas <gael.rial.costas@urjc.es>
//
// This file is part of ViSimpl <https://github.com/vg-lab/visimpl>
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 3.0 as published
// by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

//
// Created by gaelr on 5/07/23.
//

#ifndef NEON_MOUSEBUTTONEVENT_H
#define NEON_MOUSEBUTTONEVENT_H

#include "KeyboardEvent.h"

namespace neon {

    enum class MouseButton {
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

    struct MouseButtonEvent {
        MouseButton button;
        KeyboardAction action;
        int modifiers;

        [[nodiscard]] inline bool
        isModifierActive(KeyboardModifier modifier) const {
            return (modifiers & static_cast<int>(modifier)) > 0;
        }
    };

}

#endif //NEON_MOUSEBUTTONEVENT_H
