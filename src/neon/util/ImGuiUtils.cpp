//
// Created by gaeqs on 26/7/25.
//

#include "ImGuiUtils.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <neon/logging/Logger.h>

namespace
{
    struct UsingLayout
    {
        int lastFrame;
        std::unique_ptr<ImGui::neon::LinearLayout> layout;
    };

    std::unordered_map<ImGuiID, UsingLayout> _layouts;

    void eraseOldLayouts()
    {
        if (_layouts.size() < 10) {
            return;
        }
        auto it = _layouts.begin();
        while (it != _layouts.end()) {
            if (it->second.lastFrame + 10 < ImGui::GetCurrentContext()->FrameCount) {
                it = _layouts.erase(it);
            } else {
                ++it;
            }
        }
    }

    ImGui::neon::LinearLayout* getLayout(ImGuiID id)
    {
        auto it = _layouts.find(id);
        if (it == _layouts.end()) {
            return nullptr;
        }
        it->second.lastFrame = ImGui::GetCurrentContext()->FrameCount;
        return it->second.layout.get();
    }

    void pushLayout(ImGuiID id, std::unique_ptr<ImGui::neon::LinearLayout> layout)
    {
        eraseOldLayouts();
        _layouts[id] = {ImGui::GetCurrentContext()->FrameCount, std::move(layout)};
    }
} // namespace

namespace ImGui::neon
{

    void LinearLayout::space(float size)
    {
        if (getDirection() == FlowDirection::HORIZONTAL) {
            ImGui::Dummy(ImVec2(size, 0.0f));
        } else {
            ImGui::Dummy(ImVec2(0.0f, size));
        }
        next();
    }

    bool LinearLayout::button(const char* label, ImVec2 size)
    {
        bool result = Button(label, size);
        next();
        return result;
    }

    bool LinearLayout::stretchedButton(const char* label, float weight, float minSize, float perpendicularSize)
    {
        auto size = ImVec2(perpendicularSize, perpendicularSize);
        if (getDirection() == FlowDirection::HORIZONTAL) {
            size.x = popStretchedSize();
            minSize = minSize < 0.0f ? CalcTextSize(label).x + GetStyle().FramePadding.x * 2.0f : minSize;
        } else {
            size.y = popStretchedSize();
            minSize = minSize < 0.0f ? CalcTextSize(label).y + GetStyle().FramePadding.y * 2.0f : minSize;
        }
        bool result = Button(label, size);
        next(true, weight, minSize);
        return result;
    }

    void LinearLayout::text(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        TextV(fmt, args);
        va_end(args);
        next();
    }

    RowLayout::RowLayout() :
        _remainingRegionSize(0),
        _totalStretchWeight(0)
    {
    }

    FlowDirection RowLayout::getDirection() const
    {
        return FlowDirection::HORIZONTAL;
    }

    void RowLayout::begin()
    {
        _stretches.clear();

        // Compute remaining sizes and normalize weights
        float usedSize = 0.0f;
        _totalStretchWeight = 0.0f;
        for (const auto& item : _items) {
            if (!item.stretch) {
                usedSize += item.sizeOrWeight;
            } else {
                _totalStretchWeight += item.sizeOrWeight;
            }
        }

        usedSize += GetStyle().ItemSpacing.x * (_items.size() - 1);
        _remainingRegionSize = GetContentRegionAvail().x - usedSize;

        // Check for stretches that don't meet the min size condition.
        // Remove stretches with a size lower than their minimum size.
        size_t stretchesDiscarded = 0;
        do {
            stretchesDiscarded = 0;
            for (auto& item : _items) {
                if (item.stretch && !item.useMinSize) {
                    float size = _remainingRegionSize * item.sizeOrWeight / _totalStretchWeight;
                    if (size < item.minSize) {
                        // This stretch will use the defined min size and will behave as a normal element.
                        item.useMinSize = true;
                        ++stretchesDiscarded;

                        _totalStretchWeight -= item.sizeOrWeight;
                        usedSize += item.minSize;
                        _remainingRegionSize -= item.minSize;
                    }
                }
            }
        } while (stretchesDiscarded > 0);

        for (auto& item : _items) {
            if (item.stretch) {
                if (item.useMinSize) {
                    _stretches.push_back(item.minSize);
                } else {
                    _stretches.push_back(_remainingRegionSize * item.sizeOrWeight / _totalStretchWeight);
                }
            }
        }

        _items.clear();

        BeginGroup();
    }

    void RowLayout::stretch(float weight, float minSize)
    {
        ImGui::Dummy(ImVec2(popStretchedSize(), 1.0f));
        next(true, weight, minSize);
    }

    void RowLayout::next(bool stretch, float weight, float minSize)
    {
        _items.emplace_back(stretch ? weight : GetItemRectSize().x, stretch, minSize);
        SameLine();
    }

    float RowLayout::popStretchedSize()
    {
        if (_stretches.empty()) {
            return 0.0f;
        }
        float size = _stretches.front();
        _stretches.pop_front();
        return std::max(size, 0.0001f);
    }

    void RowLayout::end()
    {
        NewLine();
        EndGroup();
    }

    ColumnLayout::ColumnLayout() :
        _remainingRegionSize(0),
        _totalStretchWeight(0)
    {
    }

    FlowDirection ColumnLayout::getDirection() const
    {
        return FlowDirection::VERTICAL;
    }

    void ColumnLayout::begin()
    {
        _stretches.clear();

        // Compute remaining sizes and normalize weights
        float usedSize = 0.0f;
        _totalStretchWeight = 0.0f;
        for (const auto& item : _items) {
            if (!item.stretch) {
                usedSize += item.sizeOrWeight;
            } else {
                _totalStretchWeight += item.sizeOrWeight;
            }
        }

        usedSize += GetStyle().ItemSpacing.y * (_items.size() - 1);
        _remainingRegionSize = GetContentRegionAvail().y - usedSize;

        // Check for stretches that don't meet the min size condition.
        // Remove stretches with a size lower than their minimum size.
        size_t stretchesDiscarded = 0;
        do {
            stretchesDiscarded = 0;
            for (auto& item : _items) {
                if (item.stretch && !item.useMinSize) {
                    float size = _remainingRegionSize * item.sizeOrWeight / _totalStretchWeight;
                    if (size < item.minSize) {
                        // This stretch will use the defined min size and will behave as a normal element.
                        item.useMinSize = true;
                        ++stretchesDiscarded;

                        _totalStretchWeight -= item.sizeOrWeight;
                        usedSize += item.minSize;
                        _remainingRegionSize -= item.minSize;
                    }
                }
            }
        } while (stretchesDiscarded > 0);

        for (auto& item : _items) {
            if (item.stretch) {
                if (item.useMinSize) {
                    _stretches.push_back(item.minSize);
                } else {
                    _stretches.push_back(_remainingRegionSize * item.sizeOrWeight / _totalStretchWeight);
                }
            }
        }

        _items.clear();

        BeginGroup();
    }

    void ColumnLayout::stretch(float weight, float minSize)
    {
        ImGui::Dummy(ImVec2(1.0f, popStretchedSize()));
        next(true, weight, minSize);
    }

    void ColumnLayout::next(bool stretch, float weight, float minSize)
    {
        _items.emplace_back(stretch ? weight : GetItemRectSize().y, stretch, minSize);
    }

    float ColumnLayout::popStretchedSize()
    {
        if (_stretches.empty()) {
            return 0.0f;
        }
        float size = _stretches.front();
        _stretches.pop_front();
        return std::max(size, 0.0001f);
    }

    void ColumnLayout::end()
    {
        EndGroup();
    }

    ColumnLayout& BeginColumnLayout(const char* id)
    {
        auto imguiId = ImGui::GetID(id);
        auto layout = getLayout(imguiId);
        if (auto columnLayout = dynamic_cast<ColumnLayout*>(layout)) {
            columnLayout->begin();
            return *columnLayout;
        }

        auto newLayout = std::make_unique<ColumnLayout>();
        auto raw = newLayout.get();
        pushLayout(imguiId, std::move(newLayout));
        raw->begin();
        return *raw;
    }

    RowLayout& BeginRowLayout(const char* id)
    {
        auto imguiId = ImGui::GetID(id);
        auto layout = getLayout(imguiId);
        if (auto rowLayout = dynamic_cast<RowLayout*>(layout)) {
            rowLayout->begin();
            return *rowLayout;
        }

        auto newLayout = std::make_unique<RowLayout>();
        auto raw = newLayout.get();
        pushLayout(imguiId, std::move(newLayout));
        raw->begin();
        return *raw;
    }
} // namespace ImGui::neon
