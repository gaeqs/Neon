//
// Created by grial on 20/02/23.
//

#ifndef NEON_PHYSICSMANAGER_H
#define NEON_PHYSICSMANAGER_H

#include <cstdint>

#include <engine/Engine.h>
#include <vector>

#include "ISimulable.h"

enum class IntegrationMode {
    SYMPLETIC,
    IMPLICIT
};

class PhysicsManager : public neon::Component {

    IntegrationMode _mode;

    uint32_t _degreesOfFreedom;

    std::vector<std::unique_ptr<ISimulable>> _objects;

    void stepSympletic(float deltaTime);

public:

    PhysicsManager(IntegrationMode mode);

    void onUpdate(float deltaTime) override;

};


#endif //NEON_PHYSICSMANAGER_H
