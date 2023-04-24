//
// Created by gaelr on 24/01/2023.
//

#include "DirectionalLight.h"

#include <engine/structure/Room.h>
#include <engine/light/LightSystem.h>
#include <imgui.h>

namespace neon {

    DirectionalLight::DirectionalLight() :
            _graphicComponent(),
            _customModel(),
            _diffuseColor(1.0f, 1.0f, 1.0f),
            _specularColor(1.0f, 1.0f, 1.0f) {
    }

    DirectionalLight::DirectionalLight(const std::shared_ptr<Model>& model)
            :
            _graphicComponent(),
            _customModel(model),
            _diffuseColor(1.0f, 1.0f, 1.0f),
            _specularColor(1.0f, 1.0f, 1.0f) {
    }

    void DirectionalLight::onStart() {
        _graphicComponent = getGameObject()->newComponent<GraphicComponent>();

        if (_customModel != nullptr) {
            _graphicComponent->setModel(_customModel);
        } else {
            auto systems = getRoom()->getComponents().getComponentsOfType<LightSystem>();
            if (systems->empty()) return;
            auto model = systems->begin()->getDirectionalLightModel();
            _graphicComponent->setModel(model);
        }
    }

    void DirectionalLight::onLateUpdate(float deltaTime) {
        auto direction = getGameObject()->getTransform().getRotation()
                         * glm::vec3(0.0f, 0.0f, -1.0f);
        _graphicComponent->uploadData(Data{
                _diffuseColor,
                _specularColor,
                direction
        });
    }

    const glm::vec3& DirectionalLight::getDiffuseColor() const {
        return _diffuseColor;
    }

    void DirectionalLight::setDiffuseColor(const glm::vec3& diffuseColor) {
        _diffuseColor = diffuseColor;
    }

    const glm::vec3& DirectionalLight::getSpecularColor() const {
        return _specularColor;
    }

    void DirectionalLight::setSpecularColor(const glm::vec3& specularColor) {
        _specularColor = specularColor;
    }

    void DirectionalLight::drawEditor() {
        float w = (ImGui::GetContentRegionAvail().x -
                   ImGui::GetStyle().ItemSpacing.y) * 0.50f;

        auto lookAt = getGameObject()->getTransform().getRotation() *
                      glm::vec3(0.0f, 0.0f, -1.0f);
        ImGui::Text("Looking at: (%0.3f, %0.3f, %0.3f)", lookAt.x, lookAt.y,
                    lookAt.z);

        ImGui::PushItemWidth(w);
        ImGui::ColorPicker3(imGuiUId("##diffuse").c_str(), &_diffuseColor.x,
                            ImGuiColorEditFlags_NoSidePreview);
        ImGui::SameLine();
        ImGui::ColorPicker3(imGuiUId("##specular").c_str(), &_specularColor.x,
                            ImGuiColorEditFlags_NoSidePreview);
        ImGui::PopItemWidth();
    }
}