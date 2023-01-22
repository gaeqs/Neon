//
// Created by gaelr on 22/01/2023.
//

#include "DebugOverlayComponent.h"

#include <imgui.h>

ImPlotPoint DebugOverlayComponent::fetchProcessTime(int id, void* data) {
    auto* deque = reinterpret_cast<std::deque<float>*>(data);
    return {static_cast<double>(id), deque->at(id)};
}

DebugOverlayComponent::DebugOverlayComponent(uint32_t maxProcessTimes) :
        _maxProcessTimes(maxProcessTimes),
        _processTimes() {

    for (uint32_t i = 0; i < maxProcessTimes; ++i) {
        _processTimes.push_back(0.0f);
    }
}

void DebugOverlayComponent::onUpdate(float deltaTime) {
    constexpr float PADDING = 10.0f;

    constexpr ImGuiWindowFlags WINDOW_FLAGS =
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove;

    auto frameData = getRoom()->getApplication()->getCurrentFrameInformation();
    _processTimes.pop_front();
    _processTimes.push_back(frameData.lastFrameProcessTime);

    auto* viewport = ImGui::GetMainViewport();

    ImVec2 windowPos(
            viewport->WorkPos.x + viewport->WorkSize.x - PADDING,
            viewport->WorkPos.y + PADDING
    );
    ImVec2 windowPosPivot(1.0f, 0.0f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
    ImGui::SetNextWindowBgAlpha(0.85f);

    if (ImGui::Begin("Debug information", nullptr, WINDOW_FLAGS)) {
        ImGui::Text("Press \"L\" to enter/exit camera movement mode.");
        auto& camera = getRoom()->getCamera();
        ImGui::Text("Camera: (%f, %f, %f)",
                    camera.getPosition().x,
                    camera.getPosition().y,
                    camera.getPosition().z);

        ImGui::Separator();
        if (ImGui::TreeNode("Performance")) {
            drawPerformance();
            ImGui::TreePop();
        }

        if (ImGui::BeginPopupContextWindow()) {
            //if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void DebugOverlayComponent::drawPerformance() {
    const float MAX_DELAY_X[] = {0.0f, static_cast<float>(_maxProcessTimes)};
    const float MAX_DELAY_60_Y[] = {1.0f / 60.0f, 1.0f / 60.0f};
    const float MAX_DELAY_30_Y[] = {1.0f / 30.0f, 1.0f / 30.0f};

    auto frameData = getRoom()->getApplication()->getCurrentFrameInformation();

    ImGui::Text("Frame: %u", frameData.currentFrame);
    ImGui::Text("Delta time: %f (%f FPS)", frameData.currentDeltaTime,
                1.0f / frameData.currentDeltaTime);

    if (ImPlot::BeginPlot("Frame CPU time")) {
        ImPlot::SetupAxis(ImAxis_X1, "Frame", ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxis(ImAxis_Y1, "Time (s)", ImPlotAxisFlags_AutoFit);
        ImPlot::PlotLineG(
                "CPU time",
                fetchProcessTime,
                &_processTimes,
                static_cast<int>(_maxProcessTimes)
        );
        ImPlot::PlotLine(
                "60 FPS",
                MAX_DELAY_X,
                MAX_DELAY_60_Y,
                2
        );
        ImPlot::PlotLine(
                "30 FPS",
                MAX_DELAY_X,
                MAX_DELAY_30_Y,
                2
        );
        ImPlot::EndPlot();
    }
}
