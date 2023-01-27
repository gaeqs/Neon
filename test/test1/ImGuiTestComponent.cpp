//
// Created by gaelr on 21/01/2023.
//

#include "ImGuiTestComponent.h"

#include <imgui.h>

void ImGuiTestComponent::onStart() {
    auto& r = getRoom()->getRender();
    auto fb = r.getFrameBuffer(r.getPassesAmount() - 2);
    _frameBuffer = std::static_pointer_cast<SimpleFrameBuffer>(fb);


    auto condition = [this](const SimpleFrameBuffer* fb) {
        if (this->_windowSize.x <= 1 || this->_windowSize.y <= 1)
            return false;

        auto x = static_cast<uint32_t>(this->_windowSize.x);
        auto y = static_cast<uint32_t>(this->_windowSize.y);

        return fb->getWidth() != x || fb->getHeight() != y;
    };

    auto parameters = [this](const SimpleFrameBuffer* fb) {
        return std::make_pair(
                static_cast<uint32_t>(this->_windowSize.x),
                static_cast<uint32_t>(this->_windowSize.y));
    };

    for (uint32_t i = 0; i < r.getPassesAmount(); ++i) {
        if (auto f = std::static_pointer_cast<SimpleFrameBuffer>(
                r.getFrameBuffer(i))) {
            f->setRecreationCondition(condition);
            f->setRecreationParameters(parameters);
        }
    }
}

void ImGuiTestComponent::onUpdate(float deltaTime) {

    if (ImGui::Begin("Test")) {
        ImGui::Text("Hello world!");
    }
    ImGui::End();

    if (ImGui::Begin("Test2")) {
        _windowSize = ImGui::GetContentRegionAvail();
        if (_frameBuffer) {
            ImGui::Image(_frameBuffer->getImGuiDescriptor(0), _windowSize);
        }
    }

    ImGui::End();

    if (_windowSize.x > 0 && _windowSize.y > 0) {
        auto& c = getRoom()->getCamera();
        c.setFrustum(c.getFrustum().withAspectRatio(
                _windowSize.x / _windowSize.y));
    }

}
