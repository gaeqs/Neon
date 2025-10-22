//
// Created by gaeqs on 29/05/25.
//

#include "VKSimpleFrameBuffer.h"

#include <vulkan/AbstractVKApplication.h>
#include <vulkan/render/texture/VKSimpleTexture.h>
#include <vulkan/render/texture/VKTextureView.h>
#include <vulkan/util/VulkanConversions.h>

namespace neon::vulkan
{
    void VKSimpleFrameBuffer::initRenderPass(const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                                             const std::optional<FrameBufferDepthCreateInfo>& depthInfo)
    {
        std::vector samples(textureInfos.size(), conversions::vkSampleCountFlagBits(_samplesPerTexel));

        namespace vc = conversions;
        _renderPass = std::make_unique<VKRenderPass>(
            getApplication()->getApplication(), vc::vkFormat(textureInfos), samples, depthInfo.has_value(), false,
            _samplesPerTexel != SamplesPerTexel::COUNT_1, getApplication()->getVkDepthImageFormat(),
            conversions::vkSampleCountFlagBits(_samplesPerTexel));
    }

    void VKSimpleFrameBuffer::initOutputs(const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                                          const std::optional<FrameBufferDepthCreateInfo>& depthInfo)
    {
        _outputs.reserve(textureInfos.size());
        for (auto& createInfo : textureInfos) {
            _outputs.emplace_back(createInfo.name, createInfo, std::make_shared<MutableAsset<TextureView>>(),
                                  std::make_shared<MutableAsset<TextureView>>());
        }

        if (depthInfo.has_value()) {
            _depth = SimpleFrameBufferDepth(depthInfo->name, *depthInfo, std::make_shared<MutableAsset<TextureView>>());
        }
    }

    void VKSimpleFrameBuffer::createImages()
    {
        auto* app = getApplication()->getApplication();
        TextureCreateInfo info;
        info.width = _dimensions.x();
        info.height = _dimensions.y();
        info.depth = 1;
        info.mipmaps = 1;
        info.layers = 1;
        info.usages = {TextureUsage::COLOR_ATTACHMENT, TextureUsage::SAMPLING, TextureUsage::TRANSFER_SOURCE};

        for (auto output : _outputs) {
            auto name = output.name.value_or("");

            info.format = output.createInfo.format;
            info.layers = output.createInfo.layers;
            info.viewType = output.createInfo.viewType;
            info.samples = _samplesPerTexel;

            auto texture = std::make_shared<VKSimpleTexture>(app, name, info, nullptr);
            output.texture->emplace<VKTextureView>(app, name, TextureViewCreateInfo(), texture);

            if (_samplesPerTexel != SamplesPerTexel::COUNT_1) {
                info.samples = SamplesPerTexel::COUNT_1;
                auto resolved = std::make_shared<VKSimpleTexture>(app, name, info, nullptr);
                output.resolved->emplace<VKTextureView>(app, name, TextureViewCreateInfo{.aspect = {ViewAspect::COLOR}},
                                                        resolved);
            } else {
                output.resolved->set(output.texture->get());
            }
        }

        if (_depth) {
            auto name = _depth->name.value_or("");

            info.format = getApplication()->getDepthImageFormat();
            info.layers = 1;
            info.samples = _samplesPerTexel;
            info.usages = {TextureUsage::DEPTH_STENCIL_ATTACHMENT, TextureUsage::SAMPLING};

            auto texture = std::make_shared<VKSimpleTexture>(app, name, info, nullptr);

            TextureViewCreateInfo viewCreateInfo{.aspect = {ViewAspect::DEPTH}};

            _depth->texture->emplace<VKTextureView>(app, name, viewCreateInfo, texture);
        }
    }

    void VKSimpleFrameBuffer::createFrameBuffer()
    {
        uint32_t layers = 1;
        for (const auto& item : _outputs) {
            layers = std::min(layers, item.createInfo.layers);
        }

        std::vector<VkImageView> views;
        views.reserve(_outputs.size() * 2);

        for (auto& item : _outputs) {
            views.push_back(item.texture->as<VKTextureView>()->vk());
            if (*item.resolved != *item.texture) {
                views.push_back(item.resolved->as<VKTextureView>()->vk());
            }
        }

        if (_depth) {
            views.push_back(_depth->texture->as<VKTextureView>()->vk());
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass->getRaw();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(views.size());
        framebufferInfo.pAttachments = views.data();
        framebufferInfo.width = _dimensions.x();
        framebufferInfo.height = _dimensions.y();
        framebufferInfo.layers = layers;

        if (vkCreateFramebuffer(getApplication()->getDevice()->getRaw(), &framebufferInfo, nullptr, &_frameBuffer) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }

    void VKSimpleFrameBuffer::destroyFrameBuffer()
    {
        getApplication()->getBin()->destroyLater(getApplication()->getDevice()->getRaw(), getRuns(), _frameBuffer,
                                                 vkDestroyFramebuffer);
    }

    VKSimpleFrameBuffer::VKSimpleFrameBuffer(Application* application, rush::Vec2ui dimensions, SamplesPerTexel samples,
                                             const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                                             const std::optional<FrameBufferDepthCreateInfo>& depthInfo) :
        VKFrameBuffer(application),
        _samplesPerTexel(samples),
        _dimensions(dimensions)
    {
        initRenderPass(textureInfos, depthInfo);
        initOutputs(textureInfos, depthInfo);
        createImages();
        createFrameBuffer();
    }

    VKSimpleFrameBuffer::~VKSimpleFrameBuffer()
    {
        destroyFrameBuffer();
    }

    rush::Vec2ui VKSimpleFrameBuffer::getDimensions() const
    {
        return _dimensions;
    }

    bool VKSimpleFrameBuffer::hasDepth() const
    {
        return _depth.has_value();
    }

    uint32_t VKSimpleFrameBuffer::getColorAttachmentAmount() const
    {
        return _outputs.size();
    }

    std::vector<VkFormat> VKSimpleFrameBuffer::getColorFormats() const
    {
        std::vector<VkFormat> formats;
        formats.reserve(_outputs.size());
        for (const auto& item : _outputs) {
            formats.push_back(conversions::vkFormat(item.createInfo.format));
        }
        return formats;
    }

    std::optional<VkFormat> VKSimpleFrameBuffer::getDepthFormat() const
    {
        if (_depth) {
            return getApplication()->getVkDepthImageFormat();
        }
        return {};
    }

    VKRenderPass& VKSimpleFrameBuffer::getRenderPass()
    {
        return *_renderPass;
    }

    const VKRenderPass& VKSimpleFrameBuffer::getRenderPass() const
    {
        return *_renderPass;
    }

    SamplesPerTexel VKSimpleFrameBuffer::getSamples() const
    {
        return _samplesPerTexel;
    }

    bool VKSimpleFrameBuffer::renderImGui()
    {
        return false;
    }

    std::vector<FrameBufferOutput> VKSimpleFrameBuffer::getOutputs() const
    {
        std::vector<FrameBufferOutput> output;
        output.reserve(_outputs.size() + (_depth ? 1 : 0));
        for (const auto& item : _outputs) {
            output.emplace_back(FrameBufferOutputType::COLOR, item.texture, item.resolved);
        }
        if (_depth) {
            output.emplace_back(FrameBufferOutputType::DEPTH, _depth->texture, _depth->texture);
        }
        return output;
    }

    void* VKSimpleFrameBuffer::getNativeHandle()
    {
        return _frameBuffer;
    }

    const void* VKSimpleFrameBuffer::getNativeHandle() const
    {
        return _frameBuffer;
    }

    void VKSimpleFrameBuffer::recreate(rush::Vec2ui dimensions)
    {
        _dimensions = std::move(dimensions);
        destroyFrameBuffer();
        createImages();
        createFrameBuffer();
    }

    VkFramebuffer VKSimpleFrameBuffer::vk() const
    {
        return _frameBuffer;
    }
} // namespace neon::vulkan