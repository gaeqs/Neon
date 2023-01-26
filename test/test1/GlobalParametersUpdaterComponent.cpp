//
// Created by gaelr on 04/11/2022.
//

#include "GlobalParametersUpdaterComponent.h"

GlobalParametersUpdaterComponent::~GlobalParametersUpdaterComponent() = default;

void GlobalParametersUpdaterComponent::onStart() {
}

void GlobalParametersUpdaterComponent::onUpdate(float deltaTime) {
    auto& camera = getRoom()->getCamera();
    getRoom()->getGlobalUniformBuffer().uploadData<GlobalParameters>(
            0,
            GlobalParameters{
                    camera.getView(),
                    camera.getViewProjection(),
                    camera.getFrustum().getInverseProjection(),
                    camera.getFrustum().getNear(),
                    camera.getFrustum().getFar()
            });
}