//
// Created by gaelr on 14/12/2022.
//

#include "VKSwapChainFrameBuffer.h"

#include <stdexcept>

#include <engine/Room.h>

#include <vulkan/util/VKUtil.h>

void VKSwapChainFrameBuffer::fetchSwapChainImages() {
    uint32_t imageCount = -1;
    vkGetSwapchainImagesKHR(
            _vkApplication->getDevice(),
            _vkApplication->getSwapChain(),
            &imageCount,
            nullptr
    );
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(
            _vkApplication->getDevice(),
            _vkApplication->getSwapChain(),
            &imageCount,
            _swapChainImages.data()
    );
}

void VKSwapChainFrameBuffer::createSwapChainImageViews() {
    _swapChainImageViews.resize(_swapChainImages.size());
    for (auto i = 0; i < _swapChainImages.size(); ++i) {
        _swapChainImageViews[i] = vulkan_util::createImageView(
                _vkApplication->getDevice(),
                _swapChainImages[i],
                _vkApplication->getSwapChainImageFormat(),
                VK_IMAGE_ASPECT_COLOR_BIT
        );
    }
}

void VKSwapChainFrameBuffer::createDepthImage() {
    auto& extent = _vkApplication->getSwapChainExtent();
    auto pair = vulkan_util::createImage(
            _vkApplication->getDevice(),
            _vkApplication->getPhysicalDevice(),
            extent.width,
            extent.height,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            _vkApplication->getDepthImageFormat()
    );

    _depthImage = pair.first;
    _depthImageMemory = pair.second;

    _depthImageView = vulkan_util::createImageView(
            _vkApplication->getDevice(),
            _depthImage,
            _vkApplication->getDepthImageFormat(),
            VK_IMAGE_ASPECT_DEPTH_BIT
    );

    vulkan_util::transitionImageLayout(
            _vkApplication,
            _depthImage,
            _vkApplication->getDepthImageFormat(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );
}

void VKSwapChainFrameBuffer::createFrameBuffers() {
    auto& extent = _vkApplication->getSwapChainExtent();

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
                _vkApplication->getDevice(),
                &framebufferInfo,
                nullptr,
                &_swapChainFrameBuffers[i]
        ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
}

void VKSwapChainFrameBuffer::cleanup() {
    auto d = _vkApplication->getDevice();
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
        _vkApplication(&room->getApplication()->getImplementation()),
        _swapChainImages(),
        _swapChainImageViews(),
        _depthImage(VK_NULL_HANDLE),
        _depthImageMemory(VK_NULL_HANDLE),
        _depthImageView(VK_NULL_HANDLE),
        _swapChainFrameBuffers(),
        _renderPass(room->getApplication(),
                    {_vkApplication->getSwapChainImageFormat()},
                    depth, true, _vkApplication->getDepthImageFormat()),
        _depth(depth) {
    fetchSwapChainImages();
    createSwapChainImageViews();

    if (depth) {
        createDepthImage();
    }

    createFrameBuffers();
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
