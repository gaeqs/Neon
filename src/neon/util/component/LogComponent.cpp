// LogComponent.cpp
// (User requested code in English)

#include "LogComponent.h"

#include <filesystem>
#include <ranges>
#include <imgui.h>
#include <neon/structure/Application.h>
#include <neon/logging/ImGuiLogOutput.h>
#include <neon/logging/Logger.h>
#include <neon/util/task/Coroutine.h>

namespace neon
{
    // --- Unchanged methods ---

    float LogComponent::printLocation(const std::source_location& location)
    {
        auto path = std::filesystem::path(location.file_name());
        std::string file = path.filename().string();
        std::string line = std::to_string(location.line());

        ImGui::BeginGroup();
        bool first = false;
        for (auto part : _locationMessage.parts) {
            if (part.text == "{FILE}") {
                part.text = file;
            } else if (part.text == "{LINE}") {
                part.text = line;
            }
            if (first) {
                first = false;
            } else {
                ImGui::SameLine(0, 0);
            }
            printPart(part);
        }
        ImGui::EndGroup();

        if (!_locationMessage.parts.empty()) {
            return ImGui::GetItemRectSize().x;
        }

        return 0.0f;
    }

    float LogComponent::printGroups(const LogComponentMessage* msg)
    {
        ImGui::BeginGroup();
        float first = true;
        for (auto& group : msg->groups) {
            for (auto& prefix : group.prefix.parts) {
                if (first) {
                    first = false;
                } else {
                    ImGui::SameLine(0, 0);
                }
                printPart(prefix);
            }
        }
        ImGui::EndGroup();

        if (!first) {
            return ImGui::GetItemRectSize().x;
        }
        return 0.0f;
    }

    void LogComponent::printPart(const MessagePart& part)
    {
        std::optional<rush::Vec3f> color = {};
        for (auto effect : part.effects) {
            if (auto c = effect.toRGB(); c.has_value()) {
                color = c;
            }
        }
        if (color.has_value()) {
            auto c = color.value() / 255;
            ImVec4 imColor = {c.x(), c.y(), c.z(), 1.0f};
            ImGui::PushStyleColor(ImGuiCol_Text, imColor);
        }

        // Use TextUnformatted for a minor performance boost
        // as we don't need printf-style formatting.
        ImGui::TextUnformatted(part.text.c_str());

        if (color.has_value()) {
            ImGui::PopStyleColor();
        }
    }

    void LogComponent::refreshFilteredMessages()
    {
        if (!_filter.IsActive()) {
            return;
        }

        _filteredMessages.clear();
        _filteredMessages.reserve(_messages.size());

        for (size_t i = 0; i < _messages.size(); ++i) {
            bool filtered = false;
            auto& msg = _messages[i];
            for (const auto& [_, text] : msg.message.parts) {
                if (_filter.PassFilter(text.c_str())) {
                    _filteredMessages.push_back(i);
                    filtered = true;
                    break;
                }
            }

            if (!filtered) {
                auto path = std::filesystem::path(msg.message.sourceLocation.file_name());
                std::string file = path.filename().string();
                if (_filter.PassFilter(file.c_str())) {
                    _filteredMessages.push_back(i);
                    break;
                }

                for (const auto& group : msg.groups) {
                    if (_filter.PassFilter(group.name.c_str())) {
                        _filteredMessages.push_back(i);
                        break;
                    }
                    for (const auto& [_, text] : group.prefix.parts) {
                        if (_filter.PassFilter(text.c_str())) {
                            _filteredMessages.push_back(i);
                            break;
                        }
                    }
                }
            }
        }
    }

    void LogComponent::filterRecentMessages(size_t start)
    {
        for (size_t i = start; i < _messages.size(); ++i) {
            auto& msg = _messages[i].message;
            for (const auto& [_, text] : msg.parts) {
                if (_filter.PassFilter(text.c_str())) {
                    _filteredMessages.push_back(i);
                    break;
                }
            }
        }
    }

    LogComponent::~LogComponent()
    {
        logger.removeOutput(_outputId);
    }

    void LogComponent::onStart()
    {
        // (This method is unchanged)
        const TextEffect textEffect = TextEffect::foregroundRGB(0xFF, 0x88, 0x88);

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

        logger.addOutput(std::move(output));
    }

    // --- MODIFIED METHODS ---

    void LogComponent::onPreDraw()
    {
        ImGui::ShowDemoWindow();
        if (!ImGui::Begin("Log")) {
            ImGui::End();
            return;
        }

        // --- 1. Header Widgets ---
        if (ImGui::Button("Clear")) {
            clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &_autoScroll);
        ImGui::SameLine();
        if (_filter.Draw("Filter", -100.0f)) {
            refreshFilteredMessages();
        }

        ImGui::Separator();

        {
            std::lock_guard lock(_mutex);
            if (!_pendingMessages.empty()) {
                size_t size = _messages.size();
                _messages.insert(_messages.end(), std::make_move_iterator(_pendingMessages.begin()),
                                 std::make_move_iterator(_pendingMessages.end()));
                _pendingMessages.clear();
                filterRecentMessages(size);
            }
        }

        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(_filter.IsActive() ? _filteredMessages.size() : _messages.size()));

            float maxGroupWidth = 0.0f;
            float maxLocationWidth = 0.0f;
            while (clipper.Step()) {
                for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                    LogComponentMessage* msg;

                    if (_filter.IsActive()) {
                        msg = &_messages[_filteredMessages[row]];
                    } else {
                        msg = &_messages[row];
                    }

                    ImGui::Dummy({0.0f, 0.0f});
                    ImGui::SameLine(0.0f, 0.0f);

                    float groupsWidth = printGroups(msg);
                    ImGui::SameLine();
                    ImGui::Dummy({std::max(_cachedGroupsMaxWidth - groupsWidth, 0.0f), 0.0f});
                    ImGui::SameLine();

                    float locationWidth = printLocation(msg->message.sourceLocation);
                    ImGui::SameLine();
                    ImGui::Dummy({std::max(_cachedLocationMaxWidth - locationWidth, 0.0f), 0.0f});

                    for (auto& part : msg->message.parts) {
                        ImGui::SameLine(0, 0);
                        printPart(part);
                    }

                    maxGroupWidth = std::max(maxGroupWidth, groupsWidth);
                    maxLocationWidth = std::max(maxLocationWidth, locationWidth);
                }
            }
            clipper.End();

            if (_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }

            _cachedGroupsMaxWidth = maxGroupWidth;
            _cachedLocationMaxWidth = maxLocationWidth;
        }
        ImGui::EndChild();
        ImGui::End();
    }

    void LogComponent::addMessage(const Message& message, const std::vector<const MessageGroup*>& groups)
    {
        // This is now very fast.
        std::lock_guard lock(_mutex);

        std::vector<MessageGroup> g;
        g.reserve(groups.size());

        for (const auto* group : groups | std::ranges::views::reverse) {
            g.emplace_back(*group);
        }

        // Add to the pending list, not the main list
        _pendingMessages.emplace_back(message, std::move(g));
    }

    void LogComponent::clear()
    {
        // Must lock to clear both buffers
        std::lock_guard lock(_mutex);
        _messages.clear();
        _filteredMessages.clear();
        _pendingMessages.clear();
    }

} // namespace neon