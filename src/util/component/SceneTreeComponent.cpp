//
// Created by gaelr on 27/01/2023.
//

#include "SceneTreeComponent.h"

#include <imgui.h>

#include <engine/structure/Room.h>

SceneTreeComponent::SceneTreeComponent(
        IdentifiableWrapper<GameObjectExplorerComponent> explorer) :
        _explorer(explorer) {

}

bool SceneTreeComponent::recursiveTreePopulation(
        const std::unordered_set<IdentifiableWrapper<GameObject>>& objects) {

    bool mouseClickConsumed = false;
    for (const auto& obj: objects) {
        auto& children = obj->getChildren();
        bool selected = _explorer && _explorer->getTarget().raw() == obj;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
        if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
        if (selected) flags |= ImGuiTreeNodeFlags_Selected;


        if (ImGui::TreeNodeEx(obj->getName().c_str(), flags)) {
            mouseClickConsumed |= recursiveTreePopulation(children);
            ImGui::TreePop();
        }
        if (!mouseClickConsumed && ImGui::IsItemClicked()) {
            if (_explorer) {
                _explorer->setTarget(obj);
            }
            mouseClickConsumed = true;
        }
    }

    return mouseClickConsumed;
}

void SceneTreeComponent::onPreDraw() {
    if (ImGui::Begin("Scene tree")) {

        getRoom()->forEachGameObject([this](GameObject* obj) {
            if (obj->getParent() != nullptr) return;
            bool mouseClickConsumed = false;
            auto& children = obj->getChildren();
            bool selected = _explorer && _explorer->getTarget().raw() == obj;

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
            if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
            if (selected) flags |= ImGuiTreeNodeFlags_Selected;

            if (ImGui::TreeNodeEx(obj->getName().c_str(), flags)) {
                mouseClickConsumed = recursiveTreePopulation(children);
                ImGui::TreePop();
            }
            if (!mouseClickConsumed && ImGui::IsItemClicked()) {
                if (_explorer) {
                    _explorer->setTarget(obj);
                }
            }
        });

    }
    ImGui::End();
}
