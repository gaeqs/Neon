//
// Created by gaelr on 10/06/23.
//

#ifndef VISIMPL_VKQTSWAPCHAINFRAMEBUFFER_H
#define VISIMPL_VKQTSWAPCHAINFRAMEBUFFER_H

#ifdef USE_QT

#include <vulkan/render/VKFrameBuffer.h>
#include <vulkan/render/VKRenderPass.h>

namespace neon
{
    class Application;
}

namespace neon::vulkan
{

    class QTApplication;

    class VKQTSwapChainFrameBuffer : public VKFrameBuffer {

        QTApplication* _qtApplication;
        VKRenderPass _renderPass;

    public:

        explicit VKQTSwapChainFrameBuffer( Application* application );

        ~VKQTSwapChainFrameBuffer( ) override = default;

        [[nodiscard]] bool hasDepth() const override;

        [[nodiscard]] uint32_t getColorAttachmentAmount() const override;

        [[nodiscard]] VkFramebuffer getRaw() const override;

        [[nodiscard]] std::vector<VkFormat> getColorFormats() const override;

        [[nodiscard]] VkFormat getDepthFormat() const override;

        [[nodiscard]] const VKRenderPass& getRenderPass() const override;

        [[nodiscard]] VKRenderPass& getRenderPass() override;

        [[nodiscard]] uint32_t getWidth() const override;

        [[nodiscard]] uint32_t getHeight() const override;

        bool renderImGui() override;

    };

}

#endif

#endif //VISIMPL_VKQTSWAPCHAINFRAMEBUFFER_H
