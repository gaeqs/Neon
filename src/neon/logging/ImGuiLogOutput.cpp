//
// Created by gaeqs on 9/09/24.
//

#include "ImGuiLogOutput.h"

namespace neon
{
    ImGuiLogOutput::ImGuiLogOutput(IdentifiableWrapper<LogComponent> component) :
        _component(component)
    {
    }

    ImGuiLogOutput::~ImGuiLogOutput() = default;

    void ImGuiLogOutput::print(const Message& message, const std::vector<const MessageGroup*>& groups)
    {
        _component->addMessage(message, groups);
    }
} // namespace neon
