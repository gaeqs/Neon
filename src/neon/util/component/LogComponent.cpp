//
// Created by gaeqs on 9/09/24.
//

#include "LogComponent.h"

#include <filesystem>

#include <imgui.h>
#include <ranges>
#include <neon/Application.h>
#include <neon/logging/ImGuiLogOutput.h>
#include <neon/logging/Logger.h>
#include <neon/util/task/Coroutine.h>

namespace neon {
    bool LogComponent::printLocation(const std::source_location& location) {
        SimpleMessage message = _locationMessage;

        auto path = std::filesystem::path(location.file_name());
        std::string file = path.filename();
        std::string line = std::to_string(location.line());

        bool first = true;
        for (auto part: _locationMessage.parts) {
            if (part.text == "{FILE}") {
                part.text = file;
            } else if (part.text == "{LINE}") {
                part.text = line;
            }

            if (first) first = false;
            else ImGui::SameLine(0, 0);
            printPart(part);
        }

        return !first;
    }

    void LogComponent::printPart(const MessagePart& part) {
        std::optional<rush::Vec3f> color = {};
        for (auto effect: part.effects) {
            if (auto c = effect.toRGB(); c.has_value()) {
                color = c;
            }
        }
        if (color.has_value()) {
            auto c = color.value() / 255;
            ImVec4 imColor = {c.x(), c.y(), c.z(), 1.0f};
            ImGui::PushStyleColor(ImGuiCol_Text, imColor);
        }
        ImGui::Text(part.text.c_str());
        if (color.has_value()) {
            ImGui::PopStyleColor();
        }
    }

    LogComponent::~LogComponent() {
        getApplication()->getLogger().removeOutput(_outputId);
    }

    void LogComponent::onStart() {
        const TextEffect textEffect = TextEffect::foregroundRGB(
            0xFF, 0x88, 0x88);

        MessageBuilder builder;
        builder.push();
        builder.effect(TextEffect::foregroundRGB(0xFF, 0x00, 0x00));
        builder.effect(TextEffect::bold());
        builder.print("[");
        builder.print("{FILE}", textEffect);
        builder.print(":");
        builder.print("{LINE}", textEffect);
        builder.print("] ");
        _locationMessage = builder.buildSimple();


        auto output = std::make_unique<ImGuiLogOutput>(this);
        _outputId = output->getId();

        getApplication()->getLogger().addOutput(std::move(output));
    }

    void LogComponent::onPreDraw() {
        ImGui::ShowDemoWindow();
        if (ImGui::Begin("Log")) {
            std::unique_lock lock(_mutex);
            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(_messages.size()));

            while (clipper.Step()) {
                for (int row = clipper.DisplayStart;
                     row < clipper.DisplayEnd; row++) {
                    auto& [message, groups] = _messages.at(row);
                    bool first = !printLocation(message.sourceLocation);
                    for (auto& group: groups) {
                        for (auto& prefix: group.prefix) {
                            if (first) first = false;
                            else ImGui::SameLine(0, 0);
                            printPart(prefix);
                        }
                    }

                    if (!first) {
                        if (!message.parts.empty()) {
                            ImGui::SameLine();
                        }
                        first = true;
                    }

                    for (auto& part: message.parts) {
                        if (first) first = false;
                        else ImGui::SameLine(0, 0);
                        printPart(part);
                    }
                }
            }
            ImGui::End();
        }
    }

    void LogComponent::
    addMessage(const Message& message,
               const std::vector<const MessageGroup*>& groups) {
        std::unique_lock lock(_mutex);

        std::vector<MessageGroup> g;
        g.reserve(groups.size());

        for (const auto* group: groups | std::ranges::views::reverse) {
            g.emplace_back(*group);
        }

        _messages.emplace_back(message, g);
    }
}
