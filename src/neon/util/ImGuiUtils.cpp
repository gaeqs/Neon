//
// Created by gaeqs on 26/7/25.
//

#include "ImGuiUtils.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <vector>

namespace
{
} // namespace

namespace ImGui::neon
{

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
            minSize = minSize < 0.0f ? CalcTextSize(label).y + GetStyle().FramePadding.y * 2.0f  : minSize;
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
        _items.emplace_back(weight, true, minSize);
        SameLine(0, GetStyle().ItemSpacing.x + popStretchedSize());
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
        _items.emplace_back(weight, true, minSize);
        SetCursorPosY(GetCursorPosY() + popStretchedSize() + GetStyle().ItemSpacing.y);
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
} // namespace ImGui::neon
