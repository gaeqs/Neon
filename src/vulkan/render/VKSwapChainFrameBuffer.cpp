//
// Created by gaelr on 14/12/2022.
//

#include "VKSwapChainFrameBuffer.h"

#include <stdexcept>

#include <neon/structure/Room.h>

#include <vulkan/util/VKUtil.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/util/VulkanConversions.h>

namespace neon::vulkan
{
    void VKSwapChainFrameBuffer::fetchSwapChainImages()
    {
        uint32_t imageCount = -1;
        vkGetSwapchainImagesKHR(_vkApplication->getDevice()->getRaw(), _vkApplication->getSwapChain(), &imageCount,
                                nullptr);
        _swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(_vkApplication->getDevice()->getRaw(), _vkApplication->getSwapChain(), &imageCount,
                                _swapChainImages.data());
    }

    void VKSwapChainFrameBuffer::createSwapChainImageViews()
    {
        _swapChainImageViews.resize(_swapChainImages.size());
        for (auto i = 0; i < _swapChainImages.size(); ++i) {
            _swapChainImageViews[i] = vulkan_util::createImageView(
                _vkApplication->getDevice()->getRaw(), _swapChainImages[i], _vkApplication->getSwapChainImageFormat(),
                VK_IMAGE_ASPECT_COLOR_BIT, ImageViewCreateInfo());
        }
    }

    void VKSwapChainFrameBuffer::createColorImage()
    {
        if (_samples == SamplesPerTexel::COUNT_1) {
            return;
        }
        auto extent = _vkApplication->getSwapChainExtent();

        ImageCreateInfo info;
        info.width = extent.width;
        info.height = extent.height;
        info.samples = _samples;
        info.depth = 1;
        info.mipmaps = 1;
        info.layers = 1;
        info.usages = {TextureUsage::COLOR_ATTACHMENT, TextureUsage::TRANSFER_ATTACHMENT};

        auto pair = vulkan_util::createImage(_vkApplication->getDevice()->getRaw(),
                                             _vkApplication->getPhysicalDevice().getRaw(), info,
                                             TextureViewType::NORMAL_2D, _vkApplication->getSwapChainImageFormat());

        _colorImage = pair.first;
        _colorImageMemory = pair.second;

        _colorImageView = vulkan_util::createImageView(_vkApplication->getDevice()->getRaw(), _colorImage,
                                                       _vkApplication->getSwapChainImageFormat(),
                                                       VK_IMAGE_ASPECT_COLOR_BIT, ImageViewCreateInfo());
    }

    void VKSwapChainFrameBuffer::createDepthImage()
    {
        auto extent = _vkApplication->getSwapChainExtent();

        ImageCreateInfo info;
        info.width = extent.width;
        info.height = extent.height;
        info.samples = _samples;
        info.depth = 1;
        info.mipmaps = 1;
        info.layers = 1;
        info.usages = {TextureUsage::DEPTH_STENCIL_ATTACHMENT};

        auto pair = vulkan_util::createImage(_vkApplication->getDevice()->getRaw(),
                                             _vkApplication->getPhysicalDevice().getRaw(), info,
                                             TextureViewType::NORMAL_2D, _vkApplication->getVkDepthImageFormat());

        _depthImage = pair.first;
        _depthImageMemory = pair.second;

        _depthImageView = vulkan_util::createImageView(_vkApplication->getDevice()->getRaw(), _depthImage,
                                                       _vkApplication->getVkDepthImageFormat(),
                                                       VK_IMAGE_ASPECT_DEPTH_BIT, ImageViewCreateInfo());

        //
        {
            CommandPoolHolder holder = _vkApplication->getApplication()->getCommandManager().fetchCommandPool();
            CommandBuffer* buffer = holder.getPool().beginCommandBuffer(true);
            VkCommandBuffer rawBuffer = buffer->getImplementation().getCommandBuffer();

            vulkan_util::transitionImageLayout(_depthImage, _vkApplication->getVkDepthImageFormat(),
                                               VK_IMAGE_LAYOUT_UNDEFINED,
                                               VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1, rawBuffer);
        }
    }

    void VKSwapChainFrameBuffer::createFrameBuffers()
    {
        auto extent = _vkApplication->getSwapChainExtent();

        _swapChainFrameBuffers.resize(_swapChainImageViews.size());
        for (int i = 0; i < _swapChainImageViews.size(); ++i) {
            std::vector<VkImageView> attachments;
            if (_samples == SamplesPerTexel::COUNT_1) {
                attachments = {_swapChainImageViews[i]};
            } else {
                attachments = {_colorImageView, _swapChainImageViews[i]};
            }

            if (_depth) {
                attachments.push_back(_depthImageView);
            }

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = _renderPass.getRaw();
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = extent.width;
            framebufferInfo.height = extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(_vkApplication->getDevice()->getRaw(), &framebufferInfo, nullptr,
                                    &_swapChainFrameBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create framebuffer!");
            }
        }

        _extent = extent;
    }

    void VKSwapChainFrameBuffer::cleanup()
    {
        auto runs = getRuns();
        auto bin = _vkApplication->getBin();
        auto d = _vkApplication->getDevice()->getRaw();

        if (_samples != SamplesPerTexel::COUNT_1) {
            bin->destroyLater(d, runs, _colorImageView, vkDestroyImageView);
            bin->destroyLater(d, runs, _colorImage, vkDestroyImage);
            bin->destroyLater(d, runs, _colorImageMemory, vkFreeMemory);
        }

        bin->destroyLater(d, runs, _depthImageView, vkDestroyImageView);
        bin->destroyLater(d, runs, _depthImage, vkDestroyImage);
        bin->destroyLater(d, runs, _depthImageMemory, vkFreeMemory);

        for (auto& swapChainFramebuffer : _swapChainFrameBuffers) {
            bin->destroyLater(d, runs, swapChainFramebuffer, vkDestroyFramebuffer);
        }

        for (auto& swapChainImageView : _swapChainImageViews) {
            bin->destroyLater(d, runs, swapChainImageView, vkDestroyImageView);
        }
    }

    VKSwapChainFrameBuffer::VKSwapChainFrameBuffer(Application* application, SamplesPerTexel samples, bool depth) :
        VKFrameBuffer(application),
        _vkApplication(dynamic_cast<AbstractVKApplication*>(application->getImplementation())),
        _samples(samples),
        _depthImage(VK_NULL_HANDLE),
        _depthImageMemory(VK_NULL_HANDLE),
        _depthImageView(VK_NULL_HANDLE),
        _extent(),
        _renderPass(application, {_vkApplication->getSwapChainImageFormat()},
                    {conversions::vkSampleCountFlagBits(samples)}, depth, true, samples != SamplesPerTexel::COUNT_1,
                    _vkApplication->getVkDepthImageFormat(), conversions::vkSampleCountFlagBits(samples)),
        _swapChainCount(_vkApplication->getSwapChainCount()),
        _depth(depth)
    {
        fetchSwapChainImages();
        createSwapChainImageViews();

        createColorImage();

        if (depth) {
            createDepthImage();
        }

        createFrameBuffers();

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = _vkApplication->getInstance();
        init_info.PhysicalDevice = _vkApplication->getPhysicalDevice().getRaw();
        init_info.Device = _vkApplication->getDevice()->getRaw();
        init_info.Queue = _vkApplication->getGraphicsQueue();
        init_info.DescriptorPool = _vkApplication->getImGuiPool();
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = conversions::vkSampleCountFlagBits(_samples);
        init_info.RenderPass = _renderPass.getRaw();

        ImGui_ImplVulkan_Init(&init_info);

        auto cmd = _vkApplication->getCommandPool()->beginCommandBuffer(true);

        ImGui_ImplVulkan_CreateFontsTexture();

        cmd->end();
        cmd->submit();
        cmd->wait();
    }

    VKSwapChainFrameBuffer::~VKSwapChainFrameBuffer()
    {
        cleanup();
    }

    bool VKSwapChainFrameBuffer::hasDepth() const
    {
        return _depth;
    }

    uint32_t VKSwapChainFrameBuffer::getColorAttachmentAmount() const
    {
        return _samples == SamplesPerTexel::COUNT_1 ? 1 : 2;
    }

    void VKSwapChainFrameBuffer::recreate()
    {
        cleanup();

        fetchSwapChainImages();
        createSwapChainImageViews();

        createColorImage();

        if (_depth) {
            createDepthImage();
        }

        createFrameBuffers();
        _swapChainCount = _vkApplication->getSwapChainCount();
    }

    std::vector<VkFormat> VKSwapChainFrameBuffer::getColorFormats() const
    {
        return {_vkApplication->getSwapChainImageFormat()};
    }

    std::optional<VkFormat> VKSwapChainFrameBuffer::getDepthFormat() const
    {
        return _depth ? _vkApplication->getVkDepthImageFormat() : std::optional<VkFormat>();
    }

    void* VKSwapChainFrameBuffer::getNativeHandle()
    {
        return _swapChainFrameBuffers[_vkApplication->getCurrentSwapChainImage()];
    }

    const void* VKSwapChainFrameBuffer::getNativeHandle() const
    {
        return _swapChainFrameBuffers[_vkApplication->getCurrentSwapChainImage()];
    }

    const VKRenderPass& VKSwapChainFrameBuffer::getRenderPass() const
    {
        return _renderPass;
    }

    VKRenderPass& VKSwapChainFrameBuffer::getRenderPass()
    {
        return _renderPass;
    }

    bool VKSwapChainFrameBuffer::renderImGui()
    {
        return true;
    }

    std::vector<FrameBufferOutput> VKSwapChainFrameBuffer::getOutputs() const
    {
        FrameBufferOutput output;
        output.type = FrameBufferOutputType::SWAP;
        return {output};
    }

    SamplesPerTexel VKSwapChainFrameBuffer::getSamples() const
    {
        return _samples;
    }

    bool VKSwapChainFrameBuffer::requiresRecreation()
    {
        return _swapChainCount != _vkApplication->getSwapChainCount();
    }

    rush::Vec2ui VKSwapChainFrameBuffer::getDimensions() const
    {
        return {_extent.width, _extent.height};
    }
} // namespace neon::vulkan
