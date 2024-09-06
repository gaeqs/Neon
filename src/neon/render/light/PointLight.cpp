//
// Created by gaelr on 24/01/2023.
//

#include "PointLight.h"

#include <neon/structure/Room.h>
#include <neon/render/light/LightSystem.h>

#include <imgui.h>
#include <implot.h>

namespace neon {

    PointLight::PointLight() :
            _graphicComponent(),
            _customModel(),
            _diffuseColor(1.0f, 1.0f, 1.0f),
            _specularColor(1.0f, 1.0f, 1.0f),
            _constantAttenuation(1.0f),
            _linearAttenuation(1.0f),
            _quadraticAttenuation(1.0f),
            _radiance(1.0f) {
    }

    PointLight::PointLight(const std::shared_ptr<Model>& model) :
            _graphicComponent(),
            _customModel(model),
            _diffuseColor(1.0f, 1.0f, 1.0f),
            _specularColor(1.0f, 1.0f, 1.0f),
            _constantAttenuation(1.0f),
            _linearAttenuation(1.0f),
            _quadraticAttenuation(1.0f),
            _radiance(1.0f) {
    }

    void PointLight::onStart() {
        _graphicComponent = getGameObject()->newComponent<GraphicComponent>();

        if (_customModel != nullptr) {
            _graphicComponent->setModel(_customModel);
        } else {
            auto systems = getRoom()->getComponents().getComponentsOfType<LightSystem>();
            if (systems->empty()) return;
            auto model = systems->begin()->getPointLightModel();
            _graphicComponent->setModel(model);
        }
    }

    void PointLight::onLateUpdate(float deltaTime) {
        auto position = getGameObject()->getTransform().getPosition();
        _graphicComponent->uploadData(0, Data{
                _diffuseColor * _radiance,
                _specularColor * _radiance,
                position,
                _constantAttenuation,
                _linearAttenuation,
                _quadraticAttenuation
        });
    }

    const rush::Vec3f& PointLight::getDiffuseColor() const {
        return _diffuseColor;
    }

    void PointLight::setDiffuseColor(const rush::Vec3f& diffuseColor) {
        _diffuseColor = diffuseColor;
    }

    const rush::Vec3f& PointLight::getSpecularColor() const {
        return _specularColor;
    }

    void PointLight::setSpecularColor(const rush::Vec3f& specularColor) {
        _specularColor = specularColor;
    }

    float PointLight::getRadiance() const {
        return _radiance;
    }

    void PointLight::setRadiance(float radiance) {
        _radiance = radiance;
    }

    float PointLight::getConstantAttenuation() const {
        return _constantAttenuation;
    }

    void PointLight::setConstantAttenuation(float constantAttenuation) {
        _constantAttenuation = constantAttenuation;
    }

    float PointLight::getLinearAttenuation() const {
        return _linearAttenuation;
    }

    void PointLight::setLinearAttenuation(float linearAttenuation) {
        _linearAttenuation = linearAttenuation;
    }

    float PointLight::getQuadraticAttenuation() const {
        return _quadraticAttenuation;
    }

    void PointLight::setQuadraticAttenuation(float quadraticAttenuation) {
        _quadraticAttenuation = quadraticAttenuation;
    }

    float PointLight::attenuationAt(float distance) const {
        float att = _constantAttenuation
                    + _linearAttenuation * distance
                    + _quadraticAttenuation * distance * distance;
        return std::min(1.0f, 1.0f / att);
    }


    void PointLight::drawEditor() {
        ImGui::Text("Diffuse / Specular");

        float w = (ImGui::GetContentRegionAvail().x -
                   ImGui::GetStyle().ItemSpacing.y) * 0.50f;
        ImGui::PushItemWidth(w);
        ImGui::ColorPicker3(imGuiUId("##diffuse").c_str(),
                            _diffuseColor.toPointer(),
                            ImGuiColorEditFlags_NoSidePreview);
        ImGui::SameLine();
        ImGui::ColorPicker3(imGuiUId("##specular").c_str(),
                            _specularColor.toPointer(),
                            ImGuiColorEditFlags_NoSidePreview);
        ImGui::PopItemWidth();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Radiance:");
        ImGui::SameLine();
        ImGui::DragFloat(imGuiUId("##radiance").c_str(), &_radiance,
                         0.5f, 0.0f, 1000.0f);

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
    }
}
