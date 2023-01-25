//
// Created by gaelr on 25/01/2023.
//

#include "LightSystem.h"

LightSystem::LightSystem(
        const IdentifiableWrapper<Model>& directionalLightModel,
        const IdentifiableWrapper<Model>& pointLightModel,
        const IdentifiableWrapper<Model>& flashLightModel)
        : _directionalLightModel(directionalLightModel),
          _pointLightModel(pointLightModel),
          _flashLightModel(flashLightModel) {}

const IdentifiableWrapper<Model>&
LightSystem::getDirectionalLightModel() const {
    return _directionalLightModel;
}

const IdentifiableWrapper<Model>& LightSystem::getPointLightModel() const {
    return _pointLightModel;
}

const IdentifiableWrapper<Model>& LightSystem::getFlashLightModel() const {
    return _flashLightModel;
}
