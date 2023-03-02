//
// Created by grial on 20/02/23.
//

#ifndef NEON_PHYSICSMANAGER_H
#define NEON_PHYSICSMANAGER_H

#include <cstdint>

#include <engine/Engine.h>
#include <memory>
#include <vector>

#include "ISimulable.h"

enum class IntegrationMode {
    SYMPLECTIC,
    IMPLICIT
};

class PhysicsManager : public neon::Component {

    IntegrationMode _mode;

    uint32_t _degreesOfFreedom;

    std::vector<std::unique_ptr<ISimulable>> _objects;

    bool _paused;

    void stepSymplectic(float deltaTime);

    void stepImplicit(float deltaTime);

public:

    explicit PhysicsManager(IntegrationMode mode);

    void onUpdate(float deltaTime) override;

    void drawEditor() override;

    template<class Object, class... Params>
    Object* createSimulableObject(Params&& ... values) {
        auto ptr = std::make_unique<Object>(_degreesOfFreedom, this, values...);

        _degreesOfFreedom += ptr->getNumberOfDegreesOfFreedom();
        auto raw = ptr.get();
        _objects.emplace_back(std::move(ptr));
        return raw;
    }

};
REGISTER_COMPONENT(PhysicsManager, "Physics Manager")


#endif //NEON_PHYSICSMANAGER_H
