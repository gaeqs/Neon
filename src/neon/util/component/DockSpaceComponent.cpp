//
// Created by gaelr on 21/01/2023.
//

#include "DockSpaceComponent.h"

#include <imgui.h>

namespace neon
{
    DockSpaceComponent::DockSpaceComponent(bool topBar)
    {
        dockFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoResize;
        if (topBar) {
            dockFlags |= ImGuiWindowFlags_MenuBar;
        }
    }

    void DockSpaceComponent::onUpdate(float deltaTime)
    {
        static ImGuiID dockSpaceId = 0;
        auto& io = ImGui::GetIO();
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowPos({0, 0});

        if (ImGui::Begin("DockSpace", nullptr, dockFlags)) {
            dockSpaceId = ImGui::GetID("HUB_DockSpace");
            ImGui::DockSpace(dockSpaceId, ImVec2(0, 0), ImGuiDockNodeFlags_None);
        }

        ImGui::End();
    }
} // namespace neon
