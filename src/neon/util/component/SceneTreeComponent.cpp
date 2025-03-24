//
// Created by gaelr on 27/01/2023.
//

#include "SceneTreeComponent.h"

#include <imgui.h>

#include <neon/structure/Room.h>

namespace neon
{
    SceneTreeComponent::SceneTreeComponent(IdentifiableWrapper<GameObjectExplorerComponent> explorer) :
        _explorer(explorer),
        _roots(),
        _timeBeforePopulation(0.0f)
    {
    }

    bool SceneTreeComponent::recursiveTreePopulation(const std::unordered_set<IdentifiableWrapper<GameObject>>& objects)
    {
        bool mouseClickConsumed = false;
        for (const auto& obj : objects) {
            auto& children = obj->getChildren();
            bool selected = _explorer && _explorer->getTarget() == obj;

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
            if (children.empty()) {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }
            if (selected) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            if (ImGui::TreeNodeEx(obj.raw(), flags, "%s", obj->getName().c_str())) {
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

    void SceneTreeComponent::onPreDraw()
    {
        if (ImGui::Begin("Scene tree")) {
            _timeBeforePopulation -= getRoom()->getApplication()->getCurrentFrameInformation().currentDeltaTime;
            if (_timeBeforePopulation < 0) {
                _timeBeforePopulation = TIME_BEFORE_POPUPLATION;
                popuplate();
            }

            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(_roots.size()));

            while (clipper.Step()) {
                for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                    auto obj = _roots[row];
                    if (obj == nullptr) {
                        continue;
                    }

                    bool mouseClickConsumed = false;
                    auto& children = obj->getChildren();
                    bool selected = _explorer && _explorer->getTarget() == obj;

                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
                    if (children.empty()) {
                        flags |= ImGuiTreeNodeFlags_Leaf;
                    }
                    if (selected) {
                        flags |= ImGuiTreeNodeFlags_Selected;
                    }

                    if (ImGui::TreeNodeEx(obj.raw(), flags, "%s", obj->getName().c_str())) {
                        mouseClickConsumed = recursiveTreePopulation(children);
                        ImGui::TreePop();
                    }

                    if (ImGui::BeginPopupContextItem()) {
                        if (_explorer) {
                            _explorer->setTarget(obj);
                        }

                        if (ImGui::MenuItem("New GameObject")) {
                            getRoom()->newGameObject();
                            _timeBeforePopulation = 0;
                        }
                        ImGui::Separator();
                        if (ImGui::MenuItem("Destroy GameObject")) {
                            obj->destroy();
                        }
                        ImGui::EndPopup();
                    }

                    if (!mouseClickConsumed && ImGui::IsItemClicked()) {
                        if (_explorer) {
                            _explorer->setTarget(obj);
                        }
                    }
                }
            }
        }

        ImGui::End();
    }

    void SceneTreeComponent::popuplate()
    {
        _roots.clear();
        _roots.reserve(getRoom()->getGameObjectAmount());
        getRoom()->forEachGameObject([this](GameObject* obj) {
            if (obj->getParent() == nullptr) {
                _roots.emplace_back(obj);
            }
        });
    }
} // namespace neon