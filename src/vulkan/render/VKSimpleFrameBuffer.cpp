//
// Created by grial on 15/12/22.
//

#include "VKSimpleFrameBuffer.h"


#include <stdexcept>

#include <imgui_impl_vulkan.h>

#include <engine/structure/Room.h>
#include <engine/render/Texture.h>
#include <vulkan/util/VKUtil.h>

void VKSimpleFrameBuffer::createImages() {

    _images.clear();
    _memories.clear();
    _imageViews.clear();

    for (const auto& format: _formats) {
        auto [image, memory] = vulkan_util::createImage(
                _vkApplication->getDevice(),
                _vkApplication->getPhysicalDevice(),
                _extent.width,
                _extent.height,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                VK_IMAGE_USAGE_SAMPLED_BIT,
                format
        );

        auto view = vulkan_util::createImageView(
                _vkApplication->getDevice(),
                image,
                format,
                VK_IMAGE_ASPECT_COLOR_BIT
        );

        _images.push_back(image);
        _memories.push_back(memory);
        _imageViews.push_back(view);
        _layouts.push_back(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    // Add depth texture
    if (_depth) {
        auto [image, memory] = vulkan_util::createImage(
                _vkApplication->getDevice(),
                _vkApplication->getPhysicalDevice(),
                _extent.width,
                _extent.height,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                | VK_IMAGE_USAGE_SAMPLED_BIT,
                _vkApplication->getDepthImageFormat()
        );

        auto view = vulkan_util::createImageView(
                _vkApplication->getDevice(),
                image,
                _vkApplication->getDepthImageFormat(),
                VK_IMAGE_ASPECT_DEPTH_BIT
        );

        _images.push_back(image);
        _memories.push_back(memory);
        _imageViews.push_back(view);
        _layouts.push_back(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
    }

}

void VKSimpleFrameBuffer::createFrameBuffer() {
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = _renderPass.getRaw();
    framebufferInfo.attachmentCount = _imageViews.size();
    framebufferInfo.pAttachments = _imageViews.data();
    framebufferInfo.width = _extent.width;
    framebufferInfo.height = _extent.height;
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

    for (const auto& item: _imGuiDescriptors) {
        if (item != VK_NULL_HANDLE) {
            ImGui_ImplVulkan_RemoveTexture(item);
        }
    }
    std::fill(_imGuiDescriptors.begin(), _imGuiDescriptors.end(),
              VK_NULL_HANDLE);
}

VKSimpleFrameBuffer::VKSimpleFrameBuffer(
        Room* room,
        const std::vector<TextureFormat>& formats,
        bool depth) :
        VKFrameBuffer(),
        _vkApplication(&room->getApplication()->getImplementation()),
        _images(),
        _memories(),
        _imageViews(),
        _layouts(),
        _textures(),
        _imGuiDescriptors(),
        _formats(vulkan_util::getImageFormats(formats)),
        _extent(_vkApplication->getSwapChainExtent()),
        _renderPass(room->getApplication(), _formats, depth, false,
                    _vkApplication->getDepthImageFormat()),
        _depth(depth) {

    _imGuiDescriptors.resize(formats.size(), VK_NULL_HANDLE);

    createImages();
    createFrameBuffer();

    // Create the textures
    for (uint32_t i = 0; i < _imageViews.size(); ++i) {
        auto texture = room->getTextures().create(
                static_cast<int32_t>(_extent.width),
                static_cast<int32_t>(_extent.height),
                _imageViews[i],
                _layouts[i]
        );
        _textures.push_back(texture);
    }
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

void VKSimpleFrameBuffer::recreate(std::pair<uint32_t, uint32_t> size) {
    _extent = {size.first, size.second};

    cleanup();
    createImages();
    createFrameBuffer();

    // Refresh the textures
    for (uint32_t i = 0; i < _textures.size(); ++i) {
        _textures[i]->getImplementation().changeExternalImageView(
                static_cast<int32_t>(_extent.width),
                static_cast<int32_t>(_extent.height),
                _imageViews[i]
        );
    }
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

VKRenderPass const& VKSimpleFrameBuffer::getRenderPass() const {
    return _renderPass;
}

VKRenderPass& VKSimpleFrameBuffer::getRenderPass() {
    return _renderPass;
}

const std::vector<IdentifiableWrapper<Texture>>&
VKSimpleFrameBuffer::getTextures() const {
    return _textures;
}

bool VKSimpleFrameBuffer::renderImGui() {
    return false;
}

ImTextureID VKSimpleFrameBuffer::getImGuiDescriptor(uint32_t index) {
    if (_imGuiDescriptors[index] == VK_NULL_HANDLE) {
        auto& texture = _textures[index];
        _imGuiDescriptors[index] = ImGui_ImplVulkan_AddTexture(
                texture->getImplementation().getSampler(),
                texture->getImplementation().getImageView(),
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
    }

    return _imGuiDescriptors[index];
}

uint32_t VKSimpleFrameBuffer::getWidth() const {
    return _extent.width;
}

uint32_t VKSimpleFrameBuffer::getHeight() const {
    return _extent.height;
}

bool VKSimpleFrameBuffer::defaultRecreationCondition() const {
    auto& extent = _vkApplication->getSwapChainExtent();
    if(extent.width == 0 || extent.height == 0) return false;
    return extent.width != getWidth() || extent.height != getHeight();
}

std::pair<uint32_t, uint32_t>
VKSimpleFrameBuffer::defaultRecreationParameters() const {
    auto& extent = _vkApplication->getSwapChainExtent();
    return {extent.width, extent.height};
}