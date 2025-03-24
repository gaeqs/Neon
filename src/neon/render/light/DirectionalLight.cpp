//
// Created by gaelr on 24/01/2023.
//

#include "DirectionalLight.h"

#include <neon/structure/Room.h>
#include <neon/render/light/LightSystem.h>
#include <imgui.h>

namespace neon
{

    DirectionalLight::DirectionalLight() :
        _graphicComponent(),
        _customModel(),
        _diffuseColor(1.0f, 1.0f, 1.0f),
        _specularColor(1.0f, 1.0f, 1.0f),
        _radiance(1.0f)
    {
    }

    DirectionalLight::DirectionalLight(const std::shared_ptr<Model>& model) :
        _graphicComponent(),
        _customModel(model),
        _diffuseColor(1.0f, 1.0f, 1.0f),
        _specularColor(1.0f, 1.0f, 1.0f),
        _radiance(1.0f)
    {
    }

    void DirectionalLight::onStart()
    {
        _graphicComponent = getGameObject()->newComponent<GraphicComponent>();

        if (_customModel != nullptr) {
            _graphicComponent->setModel(_customModel);
        } else {
            auto systems = getRoom()->getComponents().getComponentsOfType<LightSystem>();
            if (systems->empty()) {
                return;
            }
            auto model = systems->begin()->getDirectionalLightModel();
            _graphicComponent->setModel(model);
        }
    }

    void DirectionalLight::onLateUpdate(float deltaTime)
    {
        auto direction = getGameObject()->getTransform().getRotation() * rush::Vec3f(0.0f, 0.0f, -1.0f);
        _graphicComponent->uploadData(0, Data{_diffuseColor * _radiance, _specularColor * _radiance, direction});
    }

    const rush::Vec3f& DirectionalLight::getDiffuseColor() const
    {
        return _diffuseColor;
    }

    void DirectionalLight::setDiffuseColor(const rush::Vec3f& diffuseColor)
    {
        _diffuseColor = diffuseColor;
    }

    const rush::Vec3f& DirectionalLight::getSpecularColor() const
    {
        return _specularColor;
    }

    void DirectionalLight::setSpecularColor(const rush::Vec3f& specularColor)
    {
        _specularColor = specularColor;
    }

    float DirectionalLight::getRadiance() const
    {
        return _radiance;
    }

    void DirectionalLight::setRadiance(float radiance)
    {
        _radiance = radiance;
    }

    void DirectionalLight::drawEditor()
    {
        float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.y) * 0.50f;

        auto lookAt = getGameObject()->getTransform().getRotation() * rush::Vec3f(0.0f, 0.0f, -1.0f);
        ImGui::Text("Looking at: (%0.3f, %0.3f, %0.3f)", lookAt.x(), lookAt.y(), lookAt.z());

        ImGui::PushItemWidth(w);
        ImGui::ColorPicker3(imGuiUId("##diffuse").c_str(), _diffuseColor.toPointer(),
                            ImGuiColorEditFlags_NoSidePreview);
        ImGui::SameLine();
        ImGui::ColorPicker3(imGuiUId("##specular").c_str(), _specularColor.toPointer(),
                            ImGuiColorEditFlags_NoSidePreview);
        ImGui::PopItemWidth();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Radiance:");
        ImGui::SameLine();
        ImGui::DragFloat(imGuiUId("##radiance").c_str(), &_radiance, 0.5f, 0.0f, 1000.0f);
    }
} // namespace neon