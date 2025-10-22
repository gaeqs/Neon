//
// Created by gaelr on 22/12/2022.
//

#include "VKRender.h"

#include <neon/structure/Application.h>
#include <neon/render/shader/Material.h>

#include <vulkan/render/VKFrameBuffer.h>
#include <vulkan/render/VKRenderPass.h>
#include <vulkan/util/VKUtil.h>

#include <vulkan/AbstractVKApplication.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

namespace neon::vulkan
{
    VKRender::VKRender(Application* application) :
        _vkApplication(dynamic_cast<AbstractVKApplication*>(application->getImplementation())),
        _drawImGui(false)
    {
    }

    void VKRender::setupFrameBufferRecreation()
    {
        vkDeviceWaitIdle(_vkApplication->getDevice()->hold());
    }

    void VKRender::beginRenderPass(const std::shared_ptr<FrameBuffer>& fb, bool clear) const
    {
        auto& cb = _vkApplication->getCurrentCommandBuffer()->getImplementation();

        vulkan_util::beginRenderPass(&cb, fb, clear);

        auto& frameBuffer = fb->getImplementation();
        _drawImGui = frameBuffer.renderImGui();
    }

    void VKRender::endRenderPass() const
    {
        auto cb = _vkApplication->getCurrentCommandBuffer()->getImplementation().getCommandBuffer();

        if (_drawImGui) {
            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb);
            _drawImGui = false;

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        }

        vkCmdEndRenderPass(cb);
    }
} // namespace neon::vulkan
