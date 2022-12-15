//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_VKSWAPCHAINFRAMEBUFFER_H
#define NEON_VKSWAPCHAINFRAMEBUFFER_H

#include <vector>

#include <vulkan/render/VKFrameBuffer.h>

class Application;

class VKApplication;

class VKSwapChainFrameBuffer : public VKFrameBuffer {

    VKApplication* _vkApplication;

    std::vector<VkImage> _swapChainImages;
    std::vector<VkImageView> _swapChainImageViews;

    VkImage _depthImage;
    VkDeviceMemory _depthImageMemory;
    VkImageView _depthImageView;

    std::vector<VkFramebuffer> _swapChainFrameBuffers;

    bool _depth;

    void fetchSwapChainImages();

    void createSwapChainImageViews();

    void createDepthImage();

    void createFrameBuffers();

    void cleanup();

public:

    VKSwapChainFrameBuffer(Application* application, bool depth);

    ~VKSwapChainFrameBuffer() override;

    [[nodiscard]] VkFramebuffer getRaw() const override;

    [[nodiscard]] bool hasDepth() override;

    [[nodiscard]] uint32_t getColorAttachmentAmount() override;

    void recreate();

};


#endif //NEON_VKSWAPCHAINFRAMEBUFFER_H
