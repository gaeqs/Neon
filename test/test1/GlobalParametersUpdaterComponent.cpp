//
// Created by gaelr on 04/11/2022.
//

#include "GlobalParametersUpdaterComponent.h"

GlobalParametersUpdaterComponent::
~GlobalParametersUpdaterComponent() noexcept {
}

void GlobalParametersUpdaterComponent::onStart() {
}

void GlobalParametersUpdaterComponent::onUpdate(float deltaTime) {
    getRoom()->getGlobalUniformBuffer().uploadData<GlobalParameters>(
            GlobalParameters{
                    getRoom()->getCamera().getViewProjection()
            });
}