//
// Created by gaelr on 04/11/2022.
//

#include "GlobalParametersUpdaterComponent.h"

GlobalParametersUpdaterComponent::
~GlobalParametersUpdaterComponent() noexcept = default;

void GlobalParametersUpdaterComponent::onStart() {
}

void GlobalParametersUpdaterComponent::onUpdate(float deltaTime) {
    getRoom()->getGlobalUniformBuffer().uploadData<GlobalParameters>(
            0,
            GlobalParameters{
                    getRoom()->getCamera().getView(),
                    getRoom()->getCamera().getViewProjection()
            });
}