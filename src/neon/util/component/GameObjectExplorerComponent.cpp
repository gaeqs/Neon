//
// Created by gaelr on 27/01/2023.
//

#include "GameObjectExplorerComponent.h"

#include <imgui.h>

namespace neon {
    const IdentifiableWrapper<GameObject>&
    GameObjectExplorerComponent::getTarget() const {
        return _target;
    }

    void GameObjectExplorerComponent::setTarget(
            const IdentifiableWrapper<GameObject>& target) {
        _target = target;
    }

    void GameObjectExplorerComponent::onPreDraw() {
        ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200),
                                            ImVec2(100000, 100000));
        if (ImGui::Begin("GameObject explorer")) {
            if (_target) {
                drawGeneralSection();
                drawTransformSection();
                drawComponentsSection();
            }
        }
        ImGui::End();
    }

    void GameObjectExplorerComponent::drawGeneralSection() const {
        constexpr size_t SIZE = 128;
        char buffer[SIZE];

        std::string name = _target->getName();
        strcpy_s(buffer, name.c_str());

        ImGui::PushItemWidth(-1.0f);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Name:");
        ImGui::SameLine();
        if (ImGui::InputText("##name", buffer, SIZE)) {
            _target->setName(std::string(buffer));
        }
        ImGui::PopItemWidth();
    }

    void GameObjectExplorerComponent::drawTransformSection() const {
        if (!ImGui::CollapsingHeader("Transform",
                                     ImGuiTreeNodeFlags_DefaultOpen))
            return;

        ImGui::PushItemWidth(-1.0f);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Position:");
        ImGui::SameLine();
        auto pos = _target->getTransform().getPosition();
        if (ImGui::DragFloat3("##position", &pos.x(), 0.1f)) {
            _target->getTransform().setPosition(pos);
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Rotation:");
        ImGui::SameLine();

        auto euler = rush::degrees(_target->getTransform()
                                           .getRotation().euler());

        if (ImGui::DragFloat3("##rotation", &euler.x(), 0.1f)) {
            auto rotation = rush::Quatf::euler(rush::radians(euler));
            _target->getTransform().setRotation(rotation);
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Scale:");
        ImGui::SameLine();
        auto scale = _target->getTransform().getScale();
        if (ImGui::DragFloat3("##scale", scale.toPointer(), 0.1f)) {
            _target->getTransform().setScale(scale);
        }

        ImGui::PopItemWidth();
    }

    void GameObjectExplorerComponent::drawComponentsSection() const {
        static ImGuiTextFilter filter;

        auto& reg = ComponentRegister::instance();

        // Copy the list. The original may be modified.
        auto list = _target->getComponents();
        for (const auto& component: list) {
            std::type_index type = typeid(*component.raw());
            auto entry = reg.getEntry(type);
            if (!entry.has_value()) continue;

            auto name = entry->name + "##" + std::to_string(component.getId());

            bool openComponent = ImGui::CollapsingHeader(name.c_str());

            if (ImGui::BeginPopupContextItem(
                    component->imGuiUId("component_context").c_str())) {
                if (ImGui::MenuItem("Destroy component")) {
                    component->destroy();
                }
                ImGui::EndPopup();
            }

            if (openComponent && component.isValid()) {
                bool enabled = component->isEnabled();
                if (ImGui::Checkbox(component->imGuiUId("Enabled").c_str(),
                                    &enabled)) {
                    component->setEnabled(enabled);
                }
                component->drawEditor();
            }
        }

        if (ImGui::Button("New component", ImVec2(-1, 0))) {
            ImGui::OpenPopup("new_component_popup");
            filter.Clear();
        }

        if (ImGui::BeginPopup("new_component_popup")) {
            filter.Draw();

            for (const auto& type: reg.getComponents()) {
                auto entry = reg.getEntry(type);

                if (!entry->creator.has_value()) continue;
                if (!filter.PassFilter(entry->name.c_str())) continue;
                if (ImGui::Selectable(entry->name.c_str())) {
                    entry->creator.value()(*_target.raw());
                }
            }

            ImGui::EndPopup();
        }
    }
}

