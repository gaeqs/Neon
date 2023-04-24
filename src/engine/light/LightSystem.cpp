//
// Created by gaelr on 25/01/2023.
//

#include "LightSystem.h"

namespace neon {
    LightSystem::LightSystem(
            const std::shared_ptr<Model>& directionalLightModel,
            const std::shared_ptr<Model>& pointLightModel,
            const std::shared_ptr<Model>& flashLightModel)
            : _directionalLightModel(directionalLightModel),
              _pointLightModel(pointLightModel),
              _flashLightModel(flashLightModel) {}

    const std::shared_ptr<Model>&
    LightSystem::getDirectionalLightModel() const {
        return _directionalLightModel;
    }

    const std::shared_ptr<Model>& LightSystem::getPointLightModel() const {
        return _pointLightModel;
    }

    const std::shared_ptr<Model>& LightSystem::getFlashLightModel() const {
        return _flashLightModel;
    }
}