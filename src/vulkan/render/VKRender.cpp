//
// Created by gaelr on 22/12/2022.
//

#include "VKRender.h"

#include <engine/Application.h>

#include <vulkan/render/VKFrameBuffer.h>
#include <vulkan/render/VKRenderPass.h>

VKRender::VKRender(Application* application) :
        _vkApplication(&application->getImplementation()) {

}

void VKRender::render(
        Room* room,
        const ClusteredLinkedCollection<RenderPassStrategy>& strategies) const {
    for (const auto& strategy: strategies) {
        auto& frameBuffer = strategy.frameBuffer->getImplementation();
        auto& renderPass = frameBuffer.getRenderPass();
        auto commandBuffer = _vkApplication->getCurrentCommandBuffer();

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass.getRaw();
        renderPassInfo.framebuffer = frameBuffer.getRaw();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = _vkApplication->getSwapChainExtent();

        std::vector<VkClearValue> clearValues;
        clearValues.resize(frameBuffer.getColorAttachmentAmount() +
                           (frameBuffer.hasDepth() ? 1 : 0));

        for (uint32_t i = 0; i < frameBuffer.getColorAttachmentAmount(); ++i) {
            clearValues[i].color = {0.0f, 0.0f, 0.0f, 1.0f};
        }

        if (frameBuffer.hasDepth()) {
            clearValues[clearValues.size() - 1].depthStencil = {1.0f, 0};
        }

        renderPassInfo.clearValueCount = clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);

        auto& extent = _vkApplication->getSwapChainExtent();

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        strategy.strategy(room, strategy.frameBuffer);

        vkCmdEndRenderPass(commandBuffer);
    }
}
