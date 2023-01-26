//
// Created by gaelr on 25/01/2023.
//

#ifndef NEON_LIGHTSYSTEM_H
#define NEON_LIGHTSYSTEM_H

#include <engine/structure/Component.h>
#include <engine/model/Model.h>

/**
 * This component storages a reference
 * too all models used by the light system.
 */
class LightSystem : public Component {

    IdentifiableWrapper<Model> _directionalLightModel;
    IdentifiableWrapper<Model> _pointLightModel;
    IdentifiableWrapper<Model> _flashLightModel;

public:

    LightSystem(const IdentifiableWrapper<Model>& directionalLightModel,
                const IdentifiableWrapper<Model>& pointLightModel,
                const IdentifiableWrapper<Model>& flashLightModel);

    const IdentifiableWrapper<Model>& getDirectionalLightModel() const;

    const IdentifiableWrapper<Model>& getPointLightModel() const;

    const IdentifiableWrapper<Model>& getFlashLightModel() const;

};


#endif //NEON_LIGHTSYSTEM_H
