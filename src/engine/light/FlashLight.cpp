//
// Created by gaelr on 24/01/2023.
//

#include "FlashLight.h"

#include <engine/structure/Room.h>
#include <engine/light/LightSystem.h>
#include <imgui.h>
#include <implot.h>

namespace neon {
    FlashLight::FlashLight() :
            _graphicComponent(),
            _customModel(),
            _diffuseColor(1.0f, 1.0f, 1.0f),
            _specularColor(1.0f, 1.0f, 1.0f),
            _constantAttenuation(1.0f),
            _linearAttenuation(1.0f),
            _quadraticAttenuation(1.0f),
            _cutOff(0.91f),
            _outerCutOff(0.82f) {
    }

    FlashLight::FlashLight(const IdentifiableWrapper<Model>& model) :
            _graphicComponent(),
            _customModel(model),
            _diffuseColor(1.0f, 1.0f, 1.0f),
            _specularColor(1.0f, 1.0f, 1.0f),
            _constantAttenuation(1.0f),
            _linearAttenuation(1.0f),
            _quadraticAttenuation(1.0f),
            _cutOff(0.91f),
            _outerCutOff(0.82f) {
    }

    void FlashLight::onStart() {
        _graphicComponent = getGameObject()->newComponent<GraphicComponent>();

        if (_customModel != nullptr) {
            _graphicComponent->setModel(_customModel);
        } else {
            auto systems = getRoom()->getComponents().getComponentsOfType<LightSystem>();
            if (systems->empty()) return;
            auto model = systems->begin()->getFlashLightModel();
            _graphicComponent->setModel(model);
        }
    }

    void FlashLight::onLateUpdate(float deltaTime) {
        auto& t = getGameObject()->getTransform();
        auto position = t.getPosition();
        auto direction = t.getRotation() * glm::vec3(0.0f, 0.0f, 1.0f);
        _graphicComponent->uploadData(Data{
                _diffuseColor,
                _specularColor,
                position,
                direction,
                _constantAttenuation,
                _linearAttenuation,
                _quadraticAttenuation,
                _cutOff,
                _outerCutOff
        });
    }

    const glm::vec3& FlashLight::getDiffuseColor() const {
        return _diffuseColor;
    }

    void FlashLight::setDiffuseColor(const glm::vec3& diffuseColor) {
        _diffuseColor = diffuseColor;
    }

    const glm::vec3& FlashLight::getSpecularColor() const {
        return _specularColor;
    }

    void FlashLight::setSpecularColor(const glm::vec3& specularColor) {
        _specularColor = specularColor;
    }

    float FlashLight::getConstantAttenuation() const {
        return _constantAttenuation;
    }

    void FlashLight::setConstantAttenuation(float constantAttenuation) {
        _constantAttenuation = constantAttenuation;
    }

    float FlashLight::getLinearAttenuation() const {
        return _linearAttenuation;
    }

    void FlashLight::setLinearAttenuation(float linearAttenuation) {
        _linearAttenuation = linearAttenuation;
    }

    float FlashLight::getQuadraticAttenuation() const {
        return _quadraticAttenuation;
    }

    void FlashLight::setQuadraticAttenuation(float quadraticAttenuation) {
        _quadraticAttenuation = quadraticAttenuation;
    }

    float FlashLight::getCutOff() const {
        return _cutOff;
    }

    void FlashLight::setCutOff(float cutOff) {
        _cutOff = cutOff;
    }

    float FlashLight::getOuterCutOff() const {
        return _outerCutOff;
    }

    void FlashLight::setOuterCutOff(float outerCutOff) {
        _outerCutOff = outerCutOff;
    }

    float FlashLight::attenuationAt(float distance) const {
        float att = _constantAttenuation
                    + _linearAttenuation * distance
                    + _quadraticAttenuation * distance * distance;
        return std::min(1.0f, 1.0f / att);
    }

    void FlashLight::drawEditor() {
        ImGui::Text("Diffuse / Specular");

        float w = (ImGui::GetContentRegionAvail().x -
                   ImGui::GetStyle().ItemSpacing.y) * 0.50f;
        ImGui::PushItemWidth(w);
        ImGui::ColorPicker3(imGuiUId("##diffuse").c_str(), &_diffuseColor.x,
                            ImGuiColorEditFlags_NoSidePreview);
        ImGui::SameLine();
        ImGui::ColorPicker3(imGuiUId("##specular").c_str(), &_specularColor.x,
                            ImGuiColorEditFlags_NoSidePreview);
        ImGui::PopItemWidth();

        ImGui::Separator();

        ImGui::PushItemWidth(-1.0f);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Constant attenuation:");
        ImGui::SameLine();
        ImGui::DragFloat(imGuiUId("##constant_attenuation").c_str(),
                         &_constantAttenuation, 0.05f);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Linear attenuation:");
        ImGui::SameLine();
        ImGui::DragFloat(imGuiUId("##linear_attenuation").c_str(),
                         &_linearAttenuation, 0.05f);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Quadratic attenuation:");
        ImGui::SameLine();
        ImGui::DragFloat(imGuiUId("##quadratic_attenuation").c_str(),
                         &_quadraticAttenuation, 0.05f);

        if (ImPlot::BeginPlot(imGuiUId("Attenuation / Distance").c_str(),
                              ImVec2(-1, 200))) {
            constexpr int MAX_VALUES = 1000;
            constexpr float STEP = 100.0F / MAX_VALUES;
            constexpr float MIN_ATTENUATION = 0.05;
            static float values[MAX_VALUES];
            int i = 0;
            for (; i < MAX_VALUES; ++i) {
                float v = attenuationAt(STEP * static_cast<float>(i));
                if (v < MIN_ATTENUATION) break;
                values[i] = v;
            }

            ImPlot::SetupAxis(ImAxis_X1, "Distance (m)",
                              ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxis(ImAxis_Y1, "Attenuation",
                              ImPlotAxisFlags_AutoFit);
            ImPlot::PlotLine(
                    imGuiUId("##attenuation").c_str(),
                    values,
                    i,
                    100.0 / MAX_VALUES
            );
            ImPlot::EndPlot();
        }

        ImGui::Separator();

        float cutOffAngle = glm::degrees(glm::acos(_cutOff));
        float outerCutOffAngle = glm::degrees(glm::acos(_outerCutOff));

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Cut off:");
        ImGui::SameLine();
        if (ImGui::DragFloat(imGuiUId("##cut_off").c_str(),
                             &cutOffAngle, 0.5f, 0.0f, 179.9f)) {
            _cutOff = glm::cos(glm::radians(cutOffAngle));
            if (_cutOff < _outerCutOff) {
                _outerCutOff = _cutOff;
            }
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Outer cut off:");
        ImGui::SameLine();
        if (ImGui::DragFloat(imGuiUId("##outer_cut_off").c_str(),
                             &outerCutOffAngle,
                             0.5f, 0.0f, 179.9f)) {
            _outerCutOff = glm::cos(glm::radians(outerCutOffAngle));
            if (_cutOff < _outerCutOff) {
                _cutOff = _outerCutOff;
            }
        }

        ImGui::PopItemWidth();
    }
}
