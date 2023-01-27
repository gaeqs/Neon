//
// Created by gaelr on 27/01/2023.
//

#include "GameObjectExplorerComponent.h"

#include <imgui.h>

const IdentifiableWrapper<GameObject>&
GameObjectExplorerComponent::getTarget() const {
    return _target;
}

void GameObjectExplorerComponent::setTarget(
        const IdentifiableWrapper<GameObject>& target) {
    _target = target;
}

namespace {

}

void GameObjectExplorerComponent::onPreDraw() {
    ImGui::ShowDemoWindow();

    if (ImGui::Begin("GameObject explorer")) {
        if (_target) {
            drawGeneralSection();
            drawTransformSection();

            for (const auto& item: _target->getComponents()) {
                ImGui::Text("%s", typeid(*item.raw()).name());
            }
        }
    }
    ImGui::End();
}

void GameObjectExplorerComponent::drawGeneralSection() const {
    ImGui::Text("%s", _target->getName().c_str());
}

void GameObjectExplorerComponent::drawTransformSection() const {
    if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::PushItemWidth(-1.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Position:");
    ImGui::SameLine();
    auto pos = _target->getTransform().getPosition();
    if (ImGui::DragFloat3("##Position", &pos.x, 0.1f)) {
        _target->getTransform().setPosition(pos);
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Rotation:");
    ImGui::SameLine();
    auto euler =glm::degrees(glm::eulerAngles(
            _target->getTransform().getRotation()));
    if (ImGui::DragFloat3("##Rotation", &euler.x, 0.1f)) {
        _target->getTransform().setRotation(glm::quat(glm::radians(euler)));
    }
    ImGui::PopItemWidth();
}


