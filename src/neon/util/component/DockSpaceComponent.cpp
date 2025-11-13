//
// Created by gaelr on 21/01/2023.
//

#include "DockSpaceComponent.h"

#include <imgui.h>

namespace neon
{

    void DockSpaceComponent::renderDockSpaceWindow(ImGuiViewport* viewport, ImVec2 pos, ImVec2 size)
    {
        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("##DockingHost", nullptr, _dockFlags)) {
            ImGuiID dockSpaceId = ImGui::GetID("HUB_DockSpace");
            ImGui::DockSpace(dockSpaceId, ImVec2(0, 0), ImGuiDockNodeFlags_None);
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    DockSpaceComponent::DockSpaceComponent() :
        _firstTime(true)
    {
        _dockFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar;
    }

    void DockSpaceComponent::onUpdate(float deltaTime)
    {
        constexpr ImGuiWindowFlags barFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
                                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                                              ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport* vp = ImGui::GetMainViewport();
        auto pos = vp->Pos;
        auto size = vp->Size;

        // First, we draw all the bars!
        for (auto& bar : _bars) {
            ImVec2 barPos, barSize;
            switch (bar->position) {
                case DockSidebarPosition::TOP:
                    barPos = pos;
                    barSize = ImVec2(size.x, bar->size);
                    pos.y += bar->size;
                    size.y -= bar->size;
                    break;
                case DockSidebarPosition::BOTTOM:
                    barPos = ImVec2(pos.x, pos.y + size.y - bar->size);
                    barSize = ImVec2(size.x, bar->size);
                    size.y -= bar->size;
                    break;
                case DockSidebarPosition::LEFT:
                    barPos = pos;
                    barSize = ImVec2(bar->size, size.y);
                    pos.x += bar->size;
                    size.x -= bar->size;
                    break;
                case DockSidebarPosition::RIGHT:
                    barPos = ImVec2(pos.x + size.x - bar->size, pos.y);
                    barSize = ImVec2(bar->size, size.y);
                    size.x -= bar->size;
                    break;
            }

            std::string name = bar->nameProvider();
            ImGui::SetWindowPos(name.c_str(), barPos);
            ImGui::SetWindowSize(name.c_str(), barSize);

            if (bar->compositor != nullptr) {
                if (ImGui::Begin(name.c_str(), nullptr, barFlags)) {
                    bar->compositor();
                }
                ImGui::End();
            }
        }

        // Now that all bars are drawn, we can draw the main dock space!
        renderDockSpaceWindow(vp, pos, size);
    }

    DockSidebar* DockSpaceComponent::addSidebar(DockSidebarPosition position, std::string name, float size,
                                                std::function<void()> compositor)
    {
        auto bar = std::make_unique<DockSidebar>(position, [name] { return name; }, size, compositor);
        return _bars.emplace_back(std::move(bar)).get();
    }

    DockSidebar* DockSpaceComponent::addSidebar(DockSidebarPosition position, std::function<std::string()> nameProvider,
                                                float size, std::function<void()> compositor)
    {
        auto bar = std::make_unique<DockSidebar>(position, nameProvider, size, compositor);
        return _bars.emplace_back(std::move(bar)).get();
    }

    bool DockSpaceComponent::removeSidebar(const DockSidebar* bar)
    {
        return std::erase_if(_bars, [bar](const std::unique_ptr<DockSidebar>& it) { return bar == it.get(); }) > 0;
    }
} // namespace neon
