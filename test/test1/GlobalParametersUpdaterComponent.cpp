//
// Created by gaelr on 04/11/2022.
//

#include "GlobalParametersUpdaterComponent.h"

GlobalParametersUpdaterComponent::
~GlobalParametersUpdaterComponent() noexcept {
    if (_buffer.isValid()) {
        getRoom()->getShaderUniformBuffers().destroy(_buffer);
    }
}

void GlobalParametersUpdaterComponent::onStart() {
    _buffer = getRoom()->getShaderUniformBuffers().create();
    _buffer->setBindingPoint(0);
}

void GlobalParametersUpdaterComponent::onUpdate(float deltaTime) {
    _buffer->uploadData(GlobalParameters {
        getRoom()->getCamera().getViewProjection()
    });
}