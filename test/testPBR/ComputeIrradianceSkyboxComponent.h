//
// Created by gaelr on 28/05/2023.
//

#ifndef NEON_COMPUTEIRRADIANCESKYBOXCOMPONENT_H
#define NEON_COMPUTEIRRADIANCESKYBOXCOMPONENT_H

#include <engine/Engine.h>

class ComputeIrradianceSkyboxComponent : public neon::Component {

    std::shared_ptr<neon::Texture> _skybox;
    std::shared_ptr<neon::Model> _screenModel;
    std::shared_ptr<neon::Material> _material;

    bool _next = false;

public:

    explicit ComputeIrradianceSkyboxComponent(
            const std::shared_ptr<neon::Texture>& skybox,
            const std::shared_ptr<neon::Model>& screenModel);

    void onStart() override;

};


#endif //NEON_COMPUTEIRRADIANCESKYBOXCOMPONENT_H
