//
// Created by grial on 17/11/22.
//

#ifndef NEON_LOCKMOUSECOMPONENT_H
#define NEON_LOCKMOUSECOMPONENT_H

#include <engine/Engine.h>

#include <util/component/CameraMovementComponent.h>

class LockMouseComponent : public Component {

    IdentifiableWrapper<CameraMovementComponent> _cameraMovementComponent;
    bool _locked = false;

    void performLock();

public:

    explicit LockMouseComponent(IdentifiableWrapper<CameraMovementComponent>
                                cameraMovementComponent);

    void onStart() override;

    void onKey(const KeyboardEvent& event) override;

};

#endif //NEON_LOCKMOUSECOMPONENT_H
