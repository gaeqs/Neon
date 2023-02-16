//
// Created by gaelr on 25/01/2023.
//

#ifndef NEON_LIGHTSYSTEM_H
#define NEON_LIGHTSYSTEM_H

#include <engine/structure/Component.h>
#include <engine/model/Model.h>

namespace neon {

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

        [[nodiscard]] const IdentifiableWrapper<Model>&
        getDirectionalLightModel() const;

        [[nodiscard]] const IdentifiableWrapper<Model>&
        getPointLightModel() const;

        [[nodiscard]] const IdentifiableWrapper<Model>&
        getFlashLightModel() const;

    };
    REGISTER_COMPONENT(LightSystem, "Light System")
}

#endif //NEON_LIGHTSYSTEM_H
