//
// Created by gaelr on 14/12/2022.
//

#include "VKSwapChainFrameBuffer.h"

#include <stdexcept>

#include <neon/structure/Room.h>

#include <vulkan/util/VKUtil.h>
#include <imgui_impl_vulkan.h>

namespace neon::vulkan {
    void VKSwapChainFrameBuffer::fetchSwapChainImages() {
        uint32_t imageCount = -1;
        vkGetSwapchainImagesKHR(
                _vkApplication->getDevice()->getRaw(),
                _vkApplication->getSwapChain(),
                &imageCount,
                nullptr
        );
        _swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(
                _vkApplication->getDevice()->getRaw(),
                _vkApplication->getSwapChain(),
                &imageCount,
                _swapChainImages.data()
        );
    }

    void VKSwapChainFrameBuffer::createSwapChainImageViews() {
        _swapChainImageViews.resize(_swapChainImages.size());
        for (auto i = 0; i < _swapChainImages.size(); ++i) {
            _swapChainImageViews[i] = vulkan_util::createImageView(
                    _vkApplication->getDevice()->getRaw(),
                    _swapChainImages[i],
                    _vkApplication->getSwapChainImageFormat(),
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    ImageViewCreateInfo()
            );
        }
    }

    void VKSwapChainFrameBuffer::createDepthImage() {
        auto extent = _vkApplication->getSwapChainExtent();

        ImageCreateInfo info;
        info.width = _extent.width;
        info.height = _extent.height;
        info.depth = 1;
        info.mipmaps = 1;
        info.layers = 1;
        info.usages = {TextureUsage::DEPTH_STENCIL_ATTACHMENT};

        auto pair = vulkan_util::createImage(
                _vkApplication->getDevice()->getRaw(),
                _vkApplication->getPhysicalDevice(),
                info,
                TextureViewType::NORMAL_2D,
                _vkApplication->getDepthImageFormat()
        );

        _depthImage = pair.first;
        _depthImageMemory = pair.second;

        _depthImageView = vulkan_util::createImageView(
                _vkApplication->getDevice()->getRaw(),
                _depthImage,
                _vkApplication->getDepthImageFormat(),
                VK_IMAGE_ASPECT_DEPTH_BIT,
                ImageViewCreateInfo()
        );

        //
        {
            CommandPoolHolder holder = _vkApplication->getApplication()
            ->getCommandManager().fetchCommandPool();
            CommandBuffer* buffer = holder.getPool().beginCommandBuffer(true);
            VkCommandBuffer rawBuffer = buffer->getImplementation().
                    getCommandBuffer();

            vulkan_util::transitionImageLayout(
                    _vkApplication,
                    _depthImage,
                    _vkApplication->getDepthImageFormat(),
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                    1, 1,
                    rawBuffer
            );
        }
    }

    void VKSwapChainFrameBuffer::createFrameBuffers() {
        auto extent = _vkApplication->getSwapChainExtent();

        _swapChainFrameBuffers.resize(_swapChainImageViews.size());
        for (int i = 0; i < _swapChainImageViews.size(); ++i) {

            VkImageView attachments[] = {
                    _swapChainImageViews[i],
                    _depthImageView
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = _renderPass.getRaw();
            framebufferInfo.attachmentCount = _depth ? 2 : 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = extent.width;
            framebufferInfo.height = extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(
                    _vkApplication->getDevice()->getRaw(),
                    &framebufferInfo,
                    nullptr,
                    &_swapChainFrameBuffers[i]
            ) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create framebuffer!");
            }
        }

        _extent = extent;
    }

    void VKSwapChainFrameBuffer::cleanup() {
        auto d = _vkApplication->getDevice()->getRaw();
        vkDestroyImageView(d, _depthImageView, nullptr);
        vkDestroyImage(d, _depthImage, nullptr);
        vkFreeMemory(d, _depthImageMemory, nullptr);

        for (auto& _swapChainFramebuffer: _swapChainFrameBuffers) {
            vkDestroyFramebuffer(d, _swapChainFramebuffer, nullptr);
        }

        for (auto& _swapChainImageView: _swapChainImageViews) {
            vkDestroyImageView(d, _swapChainImageView, nullptr);
        }
    }

    VKSwapChainFrameBuffer::VKSwapChainFrameBuffer(
            Room* room, bool depth) :
            VKFrameBuffer(),
            _vkApplication(dynamic_cast<AbstractVKApplication*>(
                                   room->getApplication()
                                           ->getImplementation())),
            _swapChainImages(),
            _swapChainImageViews(),
            _depthImage(VK_NULL_HANDLE),
            _depthImageMemory(VK_NULL_HANDLE),
            _depthImageView(VK_NULL_HANDLE),
            _swapChainFrameBuffers(),
            _extent(),
            _renderPass(room->getApplication(),
                        {_vkApplication->getSwapChainImageFormat()},
                        depth, true, _vkApplication->getDepthImageFormat()),
            _swapChainCount(_vkApplication->getSwapChainCount()),
            _depth(depth) {
        fetchSwapChainImages();
        createSwapChainImageViews();

        if (depth) {
            createDepthImage();
        }

        createFrameBuffers();

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = _vkApplication->getInstance();
        init_info.PhysicalDevice = _vkApplication->getPhysicalDevice();
        init_info.Device = _vkApplication->getDevice()->getRaw();
        init_info.Queue = _vkApplication->getGraphicsQueue();
        init_info.DescriptorPool = _vkApplication->getImGuiPool();
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info, _renderPass.getRaw());

        auto cmd = _vkApplication->getCommandPool()->beginCommandBuffer(true);

        ImGui_ImplVulkan_CreateFontsTexture(cmd->getImplementation()
            .getCommandBuffer());

        cmd->end();
        cmd->submit();
        cmd->wait();

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    VKSwapChainFrameBuffer::~VKSwapChainFrameBuffer() {
        cleanup();
    }

    VkFramebuffer VKSwapChainFrameBuffer::getRaw() const {
        return _swapChainFrameBuffers[_vkApplication->getCurrentSwapChainImage()];
    }

    bool VKSwapChainFrameBuffer::hasDepth() const {
        return _depth;
    }

    void VKSwapChainFrameBuffer::recreate() {
        cleanup();

        fetchSwapChainImages();
        createSwapChainImageViews();

        if (_depth) {
            createDepthImage();
        }

        createFrameBuffers();
        _swapChainCount = _vkApplication->getSwapChainCount();
    }

    uint32_t VKSwapChainFrameBuffer::getColorAttachmentAmount() const {
        return 1;
    }

    std::vector<VkFormat> VKSwapChainFrameBuffer::getColorFormats() const {
        return {_vkApplication->getSwapChainImageFormat()};
    }

    VkFormat VKSwapChainFrameBuffer::getDepthFormat() const {
        return _vkApplication->getDepthImageFormat();
    }

    const VKRenderPass& VKSwapChainFrameBuffer::getRenderPass() const {
        return _renderPass;
    }

    VKRenderPass& VKSwapChainFrameBuffer::getRenderPass() {
        return _renderPass;
    }

    bool VKSwapChainFrameBuffer::renderImGui() {
        return true;
    }

    uint32_t VKSwapChainFrameBuffer::getWidth() const {
        return _extent.width;
    }

    uint32_t VKSwapChainFrameBuffer::getHeight() const {
        return _extent.height;
    }

    bool VKSwapChainFrameBuffer::requiresRecreation() {
        return _swapChainCount != _vkApplication->getSwapChainCount();
    }
}