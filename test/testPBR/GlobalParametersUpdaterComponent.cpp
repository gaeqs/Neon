//
// Created by gaelr on 04/11/2022.
//

#include "GlobalParametersUpdaterComponent.h"

GlobalParametersUpdaterComponent::~GlobalParametersUpdaterComponent() = default;

void GlobalParametersUpdaterComponent::onStart() {
}

void GlobalParametersUpdaterComponent::onUpdate(float deltaTime) {
    auto& camera = getRoom()->getCamera();
    glm::vec2 screenSize(
            getApplication()->getWidth(),
            getApplication()->getHeight()
    );

    auto& buffer = getApplication()->getRender()->getGlobalUniformBuffer();

    buffer.uploadData<GlobalParameters>(
            0,
            GlobalParameters{
                    camera.getView(),
                    camera.getFrustum().getProjection(),
                    camera.getViewProjection(),
                    camera.getFrustum().getInverseProjection(),
                    screenSize,
                    camera.getFrustum().getNear(),
                    camera.getFrustum().getFar(),
            });
    buffer.uploadData<PBRParameters>(1, PBRParameters{
            metallic,
            roughness,
            useSSAO ? 1u : 0u
    });
}

void GlobalParametersUpdaterComponent::drawEditor() {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Metallic:");
    ImGui::SameLine();
    ImGui::DragFloat(imGuiUId("##metallic").c_str(), &metallic, 0.005f, 0.0f,
                     1.0f);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Roughness:");
    ImGui::SameLine();
    ImGui::DragFloat(imGuiUId("##roughness").c_str(), &roughness, 0.005f, 0.0f,
                     1.0f);
}

void GlobalParametersUpdaterComponent::onKey(const neon::KeyboardEvent& event) {
    if (event.action != neon::KeyboardAction::PRESS) return;
    if (event.key != neon::KeyboardKey::Q) return;
    useSSAO = !useSSAO;
}
