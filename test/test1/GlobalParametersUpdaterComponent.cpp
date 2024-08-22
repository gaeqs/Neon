//
// Created by gaelr on 04/11/2022.
//

#include "GlobalParametersUpdaterComponent.h"

GlobalParametersUpdaterComponent::~GlobalParametersUpdaterComponent() = default;

void GlobalParametersUpdaterComponent::onStart() {
}

void GlobalParametersUpdaterComponent::onUpdate(float deltaTime) {
        std::cout << "LOOP" << std::endl;
    auto& camera = getRoom()->getCamera();
    getApplication()->getRender()
            ->getGlobalUniformBuffer().uploadData<Matrices>(
                    0,
                    Matrices{
                            camera.getView(),
                            camera.getViewProjection(),
                            camera.getFrustum().getInverseProjection(),
                            camera.getFrustum().getNear(),
                            camera.getFrustum().getFar()
                    });
}
