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


        [[nodiscard]] std::vector<VkFormat> getColorFormats() const override;


        [[nodiscard]] const VKRenderPass& getRenderPass() const override;

        [[nodiscard]] VKRenderPass& getRenderPass() override;

        bool renderImGui() override;

        [[nodiscard]] rush::Vec2ui getDimensions() const override;

        [[nodiscard]] SamplesPerTexel getSamples() const override;

        [[nodiscard]] std::vector<FrameBufferOutput> getOutputs() const override;

        [[nodiscard]] void* getNativeHandle() override;

        [[nodiscard]] const void* getNativeHandle() const override;

        [[nodiscard]] std::optional<VkFormat> getDepthFormat() const override;
    };

}

#endif

#endif //VISIMPL_VKQTSWAPCHAINFRAMEBUFFER_H
