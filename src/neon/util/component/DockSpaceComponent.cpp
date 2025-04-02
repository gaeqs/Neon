//
// Created by gaelr on 21/01/2023.
//

#include "DockSpaceComponent.h"

#include <imgui.h>

namespace neon
{
    DockSpaceComponent::DockSpaceComponent(bool topBar) :
        _firstTime(true)
    {
        _dockFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize;
        if (topBar) {
            _dockFlags |= ImGuiWindowFlags_MenuBar;
        }
    }

    void DockSpaceComponent::onUpdate(float deltaTime)
    {
        static ImGuiID dockSpaceId = 0;
        auto& io = ImGui::GetIO();
        if (_firstTime) {
            ImGui::SetNextWindowSize(io.DisplaySize);
            ImGui::SetNextWindowPos({0, 0});
            _firstTime = false;
        }

        dockSpaceId = ImGui::GetID("HUB_DockSpace");
        ImGui::DockSpaceOverViewport(dockSpaceId, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    }
} // namespace neon
