//
// Created by gaelr on 21/01/2023.
//

#include "ImGuiTestComponent.h"

#include <imgui.h>

void ImGuiTestComponent::onUpdate(float deltaTime) {

    if (ImGui::Begin("Test")) {
        ImGui::Text("Hello world!");
    }
    ImGui::End();

    if (ImGui::Begin("Test2")) {
        auto& r = getRoom()->getRender();
        auto fb = r.getFrameBuffer(r.getPassesAmount() - 2);
        if(auto f = static_pointer_cast<SimpleFrameBuffer>(fb)) {
            ImGui::Image(f->getImGuiDescriptor(0),
                         ImVec2(fb->getWidth(), fb->getHeight()));
        }
    }
    ImGui::End();

}
