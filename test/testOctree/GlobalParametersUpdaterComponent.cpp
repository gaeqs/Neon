//
// Created by gaelr on 04/11/2022.
//

#include "GlobalParametersUpdaterComponent.h"

GlobalParametersUpdaterComponent::~GlobalParametersUpdaterComponent() = default;

void GlobalParametersUpdaterComponent::onStart() {
}

void GlobalParametersUpdaterComponent::onUpdate(float deltaTime) {
    _timestamp += deltaTime * 0.1f;
    _timestamp -= std::floor(_timestamp);

    auto& camera = getRoom()->getCamera();
    auto& buffer = getApplication()->getRender()->getGlobalUniformBuffer();

    Matrices matrices{
        camera.getView(),
        camera.getViewProjection(),
        camera.getFrustum().getProjection(),
        camera.getFrustum().getInverseProjection(),
        camera.getFrustum().getNear(),
        camera.getFrustum().getFar()
    };

    Timestamp timestamp{_timestamp};

    buffer.uploadData(0, matrices);
    buffer.uploadData(1, timestamp);
}

void GlobalParametersUpdaterComponent::drawEditor() {
    ImGui::DragFloat(imGuiUId("Timestamp").c_str(), &_timestamp,
                     0.001f, 0.0f, 1.0f);
}
