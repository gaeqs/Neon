//
// Created by gaelr on 25/01/2023.
//

#include "LightSystem.h"

#include <neon/loader/AssetLoaderHelpers.h>

CMRC_DECLARE(neon);

namespace neon
{
    LightSystem::LightSystem()
    {
    }

    LightSystem::LightSystem(const std::shared_ptr<Model>& directionalLightModel,
                             const std::shared_ptr<Model>& pointLightModel,
                             const std::shared_ptr<Model>& flashLightModel) :
        _directionalLightModel(directionalLightModel),
        _pointLightModel(pointLightModel),
        _flashLightModel(flashLightModel)
    {
    }

    void LightSystem::onStart()
    {
        CMRCFileSystem fs(cmrc::neon::get_filesystem());
        AssetLoaderContext context(getApplication(), nullptr, &fs);

        if (!_directionalLightModel) {
            _directionalLightModel = loadAssetFromFile<Model>("model/directional_light.json", context);
        }
        if (!_pointLightModel) {
            _pointLightModel = loadAssetFromFile<Model>("model/point_light.json", context);
        }
        if (!_flashLightModel) {
            _flashLightModel = loadAssetFromFile<Model>("model/flash_light.json", context);
        }
    }

    const std::shared_ptr<Model>& LightSystem::getDirectionalLightModel() const
    {
        return _directionalLightModel;
    }

    const std::shared_ptr<Model>& LightSystem::getPointLightModel() const
    {
        return _pointLightModel;
    }

    const std::shared_ptr<Model>& LightSystem::getFlashLightModel() const
    {
        return _flashLightModel;
    }
} // namespace neon