//
// Created by grial on 15/12/22.
//

#include "VKSimpleFrameBuffer.h"


#include <utility>

#include <engine/Application.h>
#include <vulkan/util/VKUtil.h>

void VKSimpleFrameBuffer::createImages() {

    _images.clear();
    _memories.clear();
    _imageViews.clear();

    auto& extent = _vkApplication->getSwapChainExtent();

    for (const auto& format: _formats) {
        auto [image, memory] = vulkan_util::createImage(
                _vkApplication->getDevice(),
                _vkApplication->getPhysicalDevice(),
                extent.width,
                extent.height,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                format
        );

        auto view = vulkan_util::createImageView(
                _vkApplication->getDevice(),
                image,
                format,
                VK_IMAGE_ASPECT_COLOR_BIT
        );

        vulkan_util::transitionImageLayout(
                _vkApplication,
                image,
                format,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        );

        _images.push_back(image);
        _memories.push_back(memory);
        _imageViews.push_back(view);
    }

    // Add depth texture
    if (_depth) {
        auto [image, memory] = vulkan_util::createImage(
                _vkApplication->getDevice(),
                _vkApplication->getPhysicalDevice(),
                extent.width,
                extent.height,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                _vkApplication->getDepthImageFormat()
        );

        auto view = vulkan_util::createImageView(
                _vkApplication->getDevice(),
                image,
                _vkApplication->getDepthImageFormat(),
                VK_IMAGE_ASPECT_DEPTH_BIT
        );

        vulkan_util::transitionImageLayout(
                _vkApplication,
                image,
                _vkApplication->getDepthImageFormat(),
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        );

        _images.push_back(image);
        _memories.push_back(memory);
        _imageViews.push_back(view);
    }

}

void VKSimpleFrameBuffer::createFrameBuffer() {
    auto& extent = _vkApplication->getSwapChainExtent();

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = _vkApplication->getRenderPass();
    framebufferInfo.attachmentCount = _imageViews.size();
    framebufferInfo.pAttachments = _imageViews.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(
            _vkApplication->getDevice(),
            &framebufferInfo,
            nullptr,
            &_frameBuffer
    ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create framebuffer!");
    }
}

void VKSimpleFrameBuffer::cleanup() {
    auto d = _vkApplication->getDevice();

    vkDestroyFramebuffer(d, _frameBuffer, nullptr);

    for (auto& view: _imageViews) {
        vkDestroyImageView(d, view, nullptr);
    }

    for (auto& image: _images) {
        vkDestroyImage(d, image, nullptr);
    }

    for (auto& memory: _memories) {
        vkFreeMemory(d, memory, nullptr);
    }
}

VKSimpleFrameBuffer::VKSimpleFrameBuffer(
        Application* application,
        std::vector<VkFormat> formats, bool depth) :
        VKFrameBuffer(),
        _vkApplication(&application->getImplementation()),
        _images(),
        _memories(),
        _imageViews(),
        _formats(std::move(formats)),
        _depth(depth) {
    createImages();
    createFrameBuffer();
}

VKSimpleFrameBuffer::~VKSimpleFrameBuffer() {
    cleanup();
}

VkFramebuffer VKSimpleFrameBuffer::getRaw() const {
    return _frameBuffer;
}

bool VKSimpleFrameBuffer::hasDepth() const {
    return _depth;
}

void VKSimpleFrameBuffer::recreate() {
    cleanup();
    createImages();
    createFrameBuffer();
}

uint32_t VKSimpleFrameBuffer::getColorAttachmentAmount() const {
    return _formats.size();
}

std::vector<VkFormat> VKSimpleFrameBuffer::getColorFormats() const {
    return _formats;
}

VkFormat VKSimpleFrameBuffer::getDepthFormat() const {
    return _vkApplication->getDepthImageFormat();
}