//
// Created by grial on 17/11/22.
//

#include "LockMouseComponent.h"

void LockMouseComponent::performLock()
{
    getRoom()->getApplication()->lockMouse(_locked);
    _cameraMovementComponent->setEnabled(_locked);
}

LockMouseComponent::LockMouseComponent(
    neon::IdentifiableWrapper<neon::CameraMovementComponent> cameraMovementComponent) :
    _cameraMovementComponent(cameraMovementComponent)
{
}

void LockMouseComponent::onStart()
{
    performLock();
}

void LockMouseComponent::onKey(const neon::KeyboardEvent& event)
{
    if (event.key == neon::KeyboardKey::L && event.action == neon::KeyboardAction::PRESS &&
        event.isModifierActive(neon::KeyboardModifier::CONTROL)) {
        _locked = !_locked;
        performLock();
    }
}
