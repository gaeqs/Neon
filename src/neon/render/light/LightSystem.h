//
// Created by gaelr on 25/01/2023.
//

#ifndef NEON_LIGHTSYSTEM_H
#define NEON_LIGHTSYSTEM_H

#include <memory>

#include <neon/structure/Component.h>
#include <neon/render/model/Model.h>

namespace neon
{

    /**
     * This component storages a reference to all models used by the light system.
     */
    class LightSystem : public Component
    {
        std::shared_ptr<Model> _directionalLightModel;
        std::shared_ptr<Model> _pointLightModel;
        std::shared_ptr<Model> _flashLightModel;

      public:
        LightSystem(const std::shared_ptr<Model>& directionalLightModel, const std::shared_ptr<Model>& pointLightModel,
                    const std::shared_ptr<Model>& flashLightModel);

        [[nodiscard]] const std::shared_ptr<Model>& getDirectionalLightModel() const;

        [[nodiscard]] const std::shared_ptr<Model>& getPointLightModel() const;

        [[nodiscard]] const std::shared_ptr<Model>& getFlashLightModel() const;
    };
    REGISTER_COMPONENT(LightSystem, "Light System")
} // namespace neon

#endif //NEON_LIGHTSYSTEM_H
