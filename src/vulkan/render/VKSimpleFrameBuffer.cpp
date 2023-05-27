//
// Created by grial on 15/12/22.
//

#include "VKSimpleFrameBuffer.h"

#include <stdexcept>

#include <imgui_impl_vulkan.h>

#include <engine/structure/Room.h>
#include <engine/render/Texture.h>
#include <vulkan/util/VKUtil.h>
#include <vulkan/util/VulkanConversions.h>

namespace neon::vulkan {
    void VKSimpleFrameBuffer::createImages(
            std::shared_ptr<Texture> overrideDepth) {

        _images.clear();
        _memories.clear();
        _imageViews.clear();

        ImageCreateInfo info;
        info.width = _extent.width;
        info.height = _extent.height;
        info.depth = 1;
        info.mipmaps = 1;
        info.layers = 1;
        info.usages = {TextureUsage::COLOR_ATTACHMENT, TextureUsage::SAMPLING};

        for (const auto& format: _formats) {
            info.format = format;
            auto [image, memory] = vulkan_util::createImage(
                    _vkApplication->getDevice(),
                    _vkApplication->getPhysicalDevice(),
                    info,
                    TextureViewType::NORMAL_2D
            );

            auto view = vulkan_util::createImageView(
                    _vkApplication->getDevice(),
                    image,
                    conversions::vkFormat(format),
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    ImageViewCreateInfo()
            );

            _images.push_back(image);
            _memories.push_back(memory);
            _imageViews.push_back(view);
            _layouts.push_back(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        // Add depth texture
        if (!_depth) return;
        if (overrideDepth == nullptr) {
            info.usages = {TextureUsage::DEPTH_STENCIL_ATTACHMENT,
                           TextureUsage::SAMPLING};
            auto [image, memory] = vulkan_util::createImage(
                    _vkApplication->getDevice(),
                    _vkApplication->getPhysicalDevice(),
                    info,
                    TextureViewType::NORMAL_2D,
                    _vkApplication->getDepthImageFormat() // Overrides info's format.
            );

            auto view = vulkan_util::createImageView(
                    _vkApplication->getDevice(),
                    image,
                    _vkApplication->getDepthImageFormat(),
                    VK_IMAGE_ASPECT_DEPTH_BIT,
                    ImageViewCreateInfo()
            );

            _images.push_back(image);
            _memories.push_back(memory);
            _imageViews.push_back(view);
            _layouts.push_back(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
        } else {
            auto& impl = overrideDepth->getImplementation();
            _images.push_back(impl.getImage());
            _memories.push_back(impl.getMemory());
            _imageViews.push_back(impl.getImageView());
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

        for (const auto& item: _imGuiDescriptors) {
            if (item != VK_NULL_HANDLE) {
                ImGui_ImplVulkan_RemoveTexture(item);
            }
        }
        std::fill(_imGuiDescriptors.begin(), _imGuiDescriptors.end(),
                  (VkDescriptorSet) VK_NULL_HANDLE);

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
            const std::vector<TextureFormat>& formats,
            std::pair<uint32_t, uint32_t> extent,
            bool depth,
            const std::vector<SamplerCreateInfo>& sampleInfos) :
            VKFrameBuffer(),
            _vkApplication(&application->getImplementation()),
            _frameBuffer(VK_NULL_HANDLE),
            _images(),
            _memories(),
            _imageViews(),
            _layouts(),
            _textures(),
            _imGuiDescriptors(),
            _formats(formats),
            _extent({extent.first, extent.second}),
            _renderPass(application,
                        conversions::vkFormat(formats), depth, false,
                        _vkApplication->getDepthImageFormat()),
            _depth(depth) {

        _imGuiDescriptors.resize(formats.size(), VK_NULL_HANDLE);

        createImages(nullptr);
        createFrameBuffer();

        SamplerCreateInfo info;
        info.anisotropy = false;
        info.minificationFilter = TextureFilter::NEAREST;
        info.magnificationFilter = TextureFilter::NEAREST;

        // Create the textures
        for (uint32_t i = 0; i < _imageViews.size(); ++i) {

            auto texture = std::make_shared<Texture>(
                    application,
                    std::to_string(i),
                    _images[i],
                    _memories[i],
                    _imageViews[i],
                    _layouts[i],
                    static_cast<int32_t>(_extent.width),
                    static_cast<int32_t>(_extent.height),
                    1,
                    i >= _formats.size() ? TextureFormat::DEPTH24STENCIL8
                                         : _formats[i],
                    i >= sampleInfos.size() ? info : sampleInfos[i]
            );

            _textures.push_back(texture);
        }
    }

    VKSimpleFrameBuffer::VKSimpleFrameBuffer(
            Application* application,
            const std::vector<TextureFormat>& formats,
            std::pair<uint32_t, uint32_t> extent,
            std::shared_ptr<Texture> depthTexture,
            const std::vector<SamplerCreateInfo>& sampleInfos) :
            VKFrameBuffer(),
            _vkApplication(&application->getImplementation()),
            _frameBuffer(VK_NULL_HANDLE),
            _images(),
            _memories(),
            _imageViews(),
            _layouts(),
            _textures(),
            _imGuiDescriptors(),
            _formats(formats),
            _extent({extent.first, extent.second}),
            _renderPass(application,
                        conversions::vkFormat(formats),
                        depthTexture != nullptr, false,
                        _vkApplication->getDepthImageFormat()),
            _depth(depthTexture != nullptr) {

        _imGuiDescriptors.resize(formats.size(), VK_NULL_HANDLE);

        createImages(depthTexture);
        createFrameBuffer();

        SamplerCreateInfo info;
        info.anisotropy = false;
        info.minificationFilter = TextureFilter::NEAREST;
        info.magnificationFilter = TextureFilter::NEAREST;

        int skip = depthTexture == nullptr ? 0 : 1;

        // Create the textures
        for (uint32_t i = 0; i < _imageViews.size() - skip; ++i) {

            auto texture = std::make_shared<Texture>(
                    application,
                    std::to_string(i),
                    _images[i],
                    _memories[i],
                    _imageViews[i],
                    _layouts[i],
                    static_cast<int32_t>(_extent.width),
                    static_cast<int32_t>(_extent.height),
                    1,
                    _formats[i],
                    i >= sampleInfos.size() ? info : sampleInfos[i]
            );

            _textures.push_back(texture);
        }

        if (depthTexture != nullptr) {
            _textures.push_back(depthTexture);
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
        createImages(nullptr);
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
        return conversions::vkFormat(_formats);
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

    const std::vector<std::shared_ptr<Texture>>&
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

        return reinterpret_cast<void*>(_imGuiDescriptors[index]);
    }

    uint32_t VKSimpleFrameBuffer::getWidth() const {
        return _extent.width;
    }

    uint32_t VKSimpleFrameBuffer::getHeight() const {
        return _extent.height;
    }

    bool VKSimpleFrameBuffer::defaultRecreationCondition() const {
        auto& extent = _vkApplication->getSwapChainExtent();
        if (extent.width == 0 || extent.height == 0) return false;
        return extent.width != getWidth() || extent.height != getHeight();
    }

    std::pair<uint32_t, uint32_t>
    VKSimpleFrameBuffer::defaultRecreationParameters() const {
        auto& extent = _vkApplication->getSwapChainExtent();
        return {extent.width, extent.height};
    }
}
