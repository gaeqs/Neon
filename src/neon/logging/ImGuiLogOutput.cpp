//
// Created by gaeqs on 9/09/24.
//

#include "ImGuiLogOutput.h"


namespace neon {
    ImGuiLogOutput::ImGuiLogOutput(
        IdentifiableWrapper<LogComponent> component) : _component(component) {}

    ImGuiLogOutput::~ImGuiLogOutput() = default;

    void ImGuiLogOutput::print(const Message& message,
                               const MessageGroup* group) {
        _component->addMessage(message, group);
    }
}
