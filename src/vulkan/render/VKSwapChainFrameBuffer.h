//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_VKSWAPCHAINFRAMEBUFFER_H
#define NEON_VKSWAPCHAINFRAMEBUFFER_H

#include <vector>

#include <vulkan/render/VKFrameBuffer.h>
#include <vulkan/render/VKRenderPass.h>

class Room;

class VKApplication;

class VKSwapChainFrameBuffer : public VKFrameBuffer {

    VKApplication* _vkApplication;

    std::vector<VkImage> _swapChainImages;
    std::vector<VkImageView> _swapChainImageViews;

    VkImage _depthImage;
    VkDeviceMemory _depthImageMemory;
    VkImageView _depthImageView;

    std::vector<VkFramebuffer> _swapChainFrameBuffers;

    VKRenderPass _renderPass;

    bool _depth;

    void fetchSwapChainImages();

    void createSwapChainImageViews();

    void createDepthImage();

    void createFrameBuffers();

    void cleanup();

public:

    VKSwapChainFrameBuffer(Room* room, bool depth);

    ~VKSwapChainFrameBuffer() override;

    [[nodiscard]] VkFramebuffer getRaw() const override;

    [[nodiscard]] bool hasDepth() const override;

    [[nodiscard]] uint32_t getColorAttachmentAmount() const override;

    [[nodiscard]] std::vector<VkFormat> getColorFormats() const override;

    [[nodiscard]] VkFormat getDepthFormat() const override;

    [[nodiscard]] const VKRenderPass& getRenderPass() const override;

    [[nodiscard]] VKRenderPass& getRenderPass() override;

    void recreate();

};


#endif //NEON_VKSWAPCHAINFRAMEBUFFER_H
