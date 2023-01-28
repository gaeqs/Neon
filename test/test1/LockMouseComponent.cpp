//
// Created by grial on 17/11/22.
//

#include "LockMouseComponent.h"

void LockMouseComponent::performLock() {
    getRoom()->getApplication()->lockMouse(_locked);
    _cameraMovementComponent->setEnabled(_locked);
}

LockMouseComponent::LockMouseComponent(
        IdentifiableWrapper<CameraMovementComponent> cameraMovementComponent) :
        _cameraMovementComponent(cameraMovementComponent),
        _locked(false) {
}

void LockMouseComponent::onStart() {
    performLock();
}

void LockMouseComponent::onKey(const KeyboardEvent& event) {
    if (event.key == KeyboardKey::L &&
        event.action == KeyboardAction::PRESS &&
        event.isModifierActive(KeyboardModifier::CONTROL)) {
        _locked = !_locked;
        performLock();
    }
}
