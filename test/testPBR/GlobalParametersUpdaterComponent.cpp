//
// Created by gaelr on 04/11/2022.
//

#include "GlobalParametersUpdaterComponent.h"

GlobalParametersUpdaterComponent::~GlobalParametersUpdaterComponent() = default;

void GlobalParametersUpdaterComponent::onStart() {
}

void GlobalParametersUpdaterComponent::onUpdate(float deltaTime) {
    auto& camera = getRoom()->getCamera();
    getApplication()->getRender()
            ->getGlobalUniformBuffer().uploadData<GlobalParameters>(
                    0,
                    GlobalParameters{
                            camera.getView(),
                            camera.getViewProjection(),
                            camera.getFrustum().getInverseProjection(),
                            camera.getFrustum().getNear(),
                            camera.getFrustum().getFar(),
                            metallic,
                            roughness
                    });
}

void GlobalParametersUpdaterComponent::drawEditor() {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Metallic:");
    ImGui::SameLine();
    ImGui::DragFloat(imGuiUId("##metallic").c_str(), &metallic, 0.005f, 0.0f, 1.0f);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Roughness:");
    ImGui::SameLine();
    ImGui::DragFloat(imGuiUId("##roughness").c_str(), &roughness, 0.005f, 0.0f, 1.0f);
}
