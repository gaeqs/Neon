//
// Created by gaelr on 22/12/2022.
//

#include "VKRender.h"

#include <engine/Application.h>
#include <engine/shader/Material.h>

#include <vulkan/render/VKFrameBuffer.h>
#include <vulkan/render/VKRenderPass.h>
#include <vulkan/util/VKUtil.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

namespace neon::vulkan {
    VKRender::VKRender(Application* application) :
            _vkApplication(&application->getImplementation()) {

    }

    void VKRender::render(
            Room* room,
            const Render* render,
            const std::vector<std::shared_ptr<Material>>& sortedMaterials,
            const std::vector<std::shared_ptr<RenderPassStrategy>>& strategies) const {
        for (const auto& strategy: strategies) {
            strategy->render(room, render, sortedMaterials);
        }
    }

    void VKRender::setupFrameBufferRecreation() {
        vkDeviceWaitIdle(_vkApplication->getDevice());
    }

    void VKRender::beginRenderPass(
            const std::shared_ptr<FrameBuffer>& fb, bool clear) const {

        vulkan_util::beginRenderPass(
                _vkApplication->getCurrentCommandBuffer(),
                fb, clear);

        auto& frameBuffer = fb->getImplementation();
        if (frameBuffer.renderImGui()) {
            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(
                    ImGui::GetDrawData(),
                    _vkApplication->getCurrentCommandBuffer());
        }
    }

    void VKRender::endRenderPass() const {
        vkCmdEndRenderPass(_vkApplication->getCurrentCommandBuffer());
    }
}
