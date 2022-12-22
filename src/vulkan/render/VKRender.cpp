//
// Created by gaelr on 22/12/2022.
//

#include "VKRender.h"

#include <engine/Application.h>

VKRender::VKRender(Application* application) :
        _vkApplication(&application->getImplementation()) {

}

void VKRender::render(
        Room* room,
        const ClusteredLinkedCollection<RenderPassStrategy>& strategies) const {
    for (const auto& strategy: strategies) {

        auto& frameBuffer = strategy.frameBuffer->getImplementation();
        auto& renderPass = strategy.renderPass.getImplementation();
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

        VkClearValue clearColors[2];
        clearColors[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearColors[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearColors;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);

        strategy.strategy(room);

        vkCmdEndRenderPass(commandBuffer);
    }
}
