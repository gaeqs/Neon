//
// Created by gaelr on 22/01/2023.
//

#include "DebugOverlayComponent.h"

#include <sstream>

#include <imgui.h>

namespace neon
{
    ImPlotPoint DebugOverlayComponent::fetchProcessTime(int id, void* data)
    {
        auto* deque = reinterpret_cast<std::deque<float>*>(data);
        return {static_cast<double>(id), deque->at(id)};
    }

    DebugOverlayComponent::DebugOverlayComponent(bool fixedMode, uint32_t maxProcessTimes) :
        _maxProcessTimes(maxProcessTimes),
        _processTimes(),
        _fixedMode(fixedMode)
    {
        for (uint32_t i = 0; i < maxProcessTimes; ++i) {
            _processTimes.push_back(0.0f);
        }
    }

    void DebugOverlayComponent::onPreDraw()
    {
        constexpr float PADDING = 10.0f;

        const ImGuiWindowFlags WINDOW_FLAGS =
            _fixedMode ? ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                             ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking
                       : ImGuiWindowFlags_None;

        auto frameData = getRoom()->getApplication()->getCurrentFrameInformation();
        _processTimes.pop_front();
        _processTimes.push_back(frameData.lastFrameProcessTime);

        if (_fixedMode) {
            auto* viewport = ImGui::GetMainViewport();
            ImVec2 windowPos(viewport->WorkPos.x + viewport->WorkSize.x - PADDING, viewport->WorkPos.y + PADDING);
            ImVec2 windowPosPivot(1.0f, 0.0f);

            ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
            ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 0), ImVec2(-1, viewport->WorkSize.y * 0.5f));
            ImGui::SetNextWindowBgAlpha(0.85f);
        }

        if (ImGui::Begin("Debug information", nullptr, WINDOW_FLAGS)) {
            ImGui::TextWrapped("Press \"Ctrl+L\" to enter/exit camera movement mode.");
            auto& camera = getRoom()->getCamera();
            ImGui::Text("Camera: (%f, %f, %f)", camera.getPosition().x(), camera.getPosition().y(),
                        camera.getPosition().z());

            auto lookAt = camera.getForward();
            ImGui::Text("LookAt: (%f, %f, %f)", lookAt.x(), lookAt.y(), lookAt.z());

            ImGui::Separator();
            if (ImGui::TreeNode("Performance")) {
                drawPerformance();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Profiling")) {
                drawProfiling();
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    void DebugOverlayComponent::drawPerformance()
    {
        const float MAX_DELAY_X[] = {0.0f, static_cast<float>(_maxProcessTimes)};
        const float MAX_DELAY_60_Y[] = {1.0f / 60.0f, 1.0f / 60.0f};
        const float MAX_DELAY_30_Y[] = {1.0f / 30.0f, 1.0f / 30.0f};

        auto frameData = getRoom()->getApplication()->getCurrentFrameInformation();

        ImGui::Text("Frame: %u", frameData.currentFrame);
        ImGui::Text("Delta time: %f (%f FPS)", frameData.currentDeltaTime, 1.0f / frameData.currentDeltaTime);

        if (ImPlot::BeginPlot("Frame CPU time")) {
            ImPlot::SetupAxis(ImAxis_X1, "Frame", ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxis(ImAxis_Y1, "Time (s)", ImPlotAxisFlags_AutoFit);
            ImPlot::PlotLineG("CPU time", fetchProcessTime, &_processTimes, static_cast<int>(_maxProcessTimes));
            ImPlot::PlotLine("60 FPS", MAX_DELAY_X, MAX_DELAY_60_Y, 2);
            ImPlot::PlotLine("30 FPS", MAX_DELAY_X, MAX_DELAY_30_Y, 2);
            ImPlot::EndPlot();
        }
    }

    void DebugOverlayComponent::drawProfiling()
    {
        auto& profiler = getRoom()->getApplication()->getProfiler();
        auto lock = profiler.lockProfiles();

        for (const auto& [id, stack] : profiler.getProfiles()) {
            std::stringstream ss;
            ss << id;
            std::string idString = ss.str();

            if (ImGui::TreeNode(idString.c_str(), "Thread %s", idString.c_str())) {
                for (const auto& [name, s] : stack->getChildren()) {
                    drawStack(idString, s.get());
                }
                ImGui::TreePop();
            }
        }
    }

    void DebugOverlayComponent::drawStack(const std::string& parentId, ProfileStack* stack)
    {
        std::string id = parentId + "_" + stack->getName();
        auto average = stack->getAverageDuration();
        auto ms = static_cast<double>(average.count()) * 0.000001;

        if (ImGui::TreeNode(id.c_str(), "%s (%.3f ms)", stack->getName().c_str(), ms)) {
            for (const auto& [name, s] : stack->getChildren()) {
                drawStack(id, s.get());
            }
            ImGui::TreePop();
        }
    }
} // namespace neon
