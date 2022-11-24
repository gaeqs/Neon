//
// Created by grial on 17/11/22.
//

#include "LockMouseComponent.h"

void LockMouseComponent::onKey(const KeyboardEvent& event) {
    if (event.key == KeyboardKey::L && event.action == KeyboardAction::PRESS) {
        _locked = !_locked;
        getRoom()->getApplication()->lockMouse(_locked);
    }
}
