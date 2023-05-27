//
// Created by gaelr on 22/12/2022.
//

#include "VKRender.h"

#include <engine/Application.h>
#include <engine/shader/Material.h>

#include <vulkan/render/VKFrameBuffer.h>
#include <vulkan/render/VKRenderPass.h>

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
            const std::shared_ptr<FrameBuffer>& fb) const {
        auto& frameBuffer = fb->getImplementation();
        auto& renderPass = frameBuffer.getRenderPass();
        auto commandBuffer = _vkApplication->getCurrentCommandBuffer();

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass.getRaw();
        renderPassInfo.framebuffer = frameBuffer.getRaw();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {
                frameBuffer.getWidth(),
                frameBuffer.getHeight()
        };

        std::vector<VkClearValue> clearValues;
        clearValues.resize(frameBuffer.getColorAttachmentAmount() +
                           (frameBuffer.hasDepth() ? 1 : 0));

        for (uint32_t i = 0; i < frameBuffer.getColorAttachmentAmount();
             ++i) {
            clearValues[i].color = {0.0f, 0.0f, 0.0f, 1.0f};
        }

        if (frameBuffer.hasDepth()) {
            clearValues[clearValues.size() - 1].depthStencil = {1.0f, 0};
        }

        renderPassInfo.clearValueCount = clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(frameBuffer.getWidth());
        viewport.height = static_cast<float>(frameBuffer.getHeight());
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = {frameBuffer.getWidth(), frameBuffer.getHeight()};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        if (frameBuffer.renderImGui()) {
            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                            commandBuffer);
        }
    }

    void VKRender::endRenderPass() const {
        vkCmdEndRenderPass(_vkApplication->getCurrentCommandBuffer());
    }
}
