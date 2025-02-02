//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_VKSWAPCHAINFRAMEBUFFER_H
#define NEON_VKSWAPCHAINFRAMEBUFFER_H

#include <vector>

#include <vulkan/render/VKFrameBuffer.h>
#include <vulkan/render/VKRenderPass.h>

namespace neon {
    class Application;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKSwapChainFrameBuffer : public VKFrameBuffer {
        AbstractVKApplication* _vkApplication;
        SamplesPerTexel _samples;

        std::vector<VkImage> _swapChainImages;
        std::vector<VkImageView> _swapChainImageViews;

        VkImage _colorImage;
        VkDeviceMemory _colorImageMemory;
        VkImageView _colorImageView;

        VkImage _depthImage;
        VkDeviceMemory _depthImageMemory;
        VkImageView _depthImageView;

        std::vector<VkFramebuffer> _swapChainFrameBuffers;
        VkExtent2D _extent;

        VKRenderPass _renderPass;

        uint32_t _swapChainCount;

        bool _depth;

        void fetchSwapChainImages();

        void createSwapChainImageViews();

        void createColorImage();

        void createDepthImage();

        void createFrameBuffers();

        void cleanup();

    public:
        VKSwapChainFrameBuffer(Application* application, SamplesPerTexel samples, bool depth);

        ~VKSwapChainFrameBuffer() override;

        [[nodiscard]] VkFramebuffer getRaw() const override;

        [[nodiscard]] bool hasDepth() const override;

        [[nodiscard]] uint32_t getColorAttachmentAmount() const override;

        [[nodiscard]] std::vector<VkFormat> getColorFormats() const override;

        [[nodiscard]] VkFormat getDepthFormat() const override;

        [[nodiscard]] const VKRenderPass& getRenderPass() const override;

        [[nodiscard]] VKRenderPass& getRenderPass() override;

        [[nodiscard]] uint32_t getWidth() const override;

        [[nodiscard]] uint32_t getHeight() const override;

        [[nodiscard]] SamplesPerTexel getSamples() const override;

        [[nodiscard]] bool renderImGui() override;

        [[nodiscard]] bool requiresRecreation();

        void recreate();
    };
}

#endif //NEON_VKSWAPCHAINFRAMEBUFFER_H
