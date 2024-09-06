//
// Created by grial on 17/11/22.
//

#ifndef NEON_LOCKMOUSECOMPONENT_H
#define NEON_LOCKMOUSECOMPONENT_H

#include <neon/Neon.h>

#include <neon/util/component/CameraMovementComponent.h>

class LockMouseComponent : public neon::Component {

    neon::IdentifiableWrapper<neon::CameraMovementComponent>
            _cameraMovementComponent;
    bool _locked = false;

    void performLock();

public:

    explicit LockMouseComponent(
            neon::IdentifiableWrapper<neon::CameraMovementComponent> cameraMovementComponent);

    void onStart() override;

    void onKey(const neon::KeyboardEvent& event) override;

};

#endif //NEON_LOCKMOUSECOMPONENT_H
