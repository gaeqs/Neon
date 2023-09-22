//
// Created by gaelr on 04/11/2022.
//

#include "GlobalParametersUpdaterComponent.h"

GlobalParametersUpdaterComponent::~GlobalParametersUpdaterComponent() = default;

void GlobalParametersUpdaterComponent::onStart() {
}

void GlobalParametersUpdaterComponent::onUpdate(float deltaTime) {
    auto& camera = getRoom()->getCamera();
    rush::Vec2f screenSize(
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
    buffer.uploadData<PBRParameters>(1, _pbr);
}

void GlobalParametersUpdaterComponent::drawEditor() {

    ImGui::Separator();
    ImGui::Text("PBR");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Metallic:");
    ImGui::SameLine();
    ImGui::DragFloat(imGuiUId("##metallic").c_str(), &_pbr.metallic,
                     0.005f, 0.0f, 1.0f);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Roughness:");
    ImGui::SameLine();
    ImGui::DragFloat(imGuiUId("##roughness").c_str(), &_pbr.roughness,
                     0.005f, 0.0f, 1.0f);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Skybox LOD:");
    ImGui::SameLine();
    ImGui::DragFloat(imGuiUId("##skyboxLod").c_str(), &_pbr.skyboxLod,
                     0.05f, 0.0f, 10.0f);

    ImGui::Separator();
    ImGui::Text("SSAO");

    ImGui::Checkbox(imGuiUId("Enabled##SSAO").c_str(),
                    reinterpret_cast<bool*>(&_pbr.useSSAO));

    ImGui::Checkbox(imGuiUId("Show only##showSSAO").c_str(),
                    reinterpret_cast<bool*>(&_pbr.shoOnlySSAO));

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Blur radius:");
    ImGui::SameLine();
    ImGui::DragInt(imGuiUId("##ssaoFilterRadius").c_str(),
                   reinterpret_cast<int*>(&_pbr.ssaoFilterRadius),
                   0.2f, 0, 10);

    ImGui::Separator();
    ImGui::Text("Bloom");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Intensity:");
    ImGui::SameLine();
    ImGui::DragFloat(imGuiUId("##bloomIntensity").c_str(), &_pbr.bloomIntensity,
                     0.005f, 0.0f, 1.0f);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Filter radius:");
    ImGui::SameLine();
    ImGui::DragFloat(imGuiUId("##bloomFilterRadius").c_str(),
                     &_pbr.bloomFilterRadius,
                     0.0005f, 0.0f, 0.5f);
}

void GlobalParametersUpdaterComponent::onKey(const neon::KeyboardEvent& event) {
    if (event.action != neon::KeyboardAction::PRESS) return;
}
