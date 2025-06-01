//
// Created by gaeqs on 29/05/25.
//

#include "VKSimpleFrameBuffer.h"

#include <vulkan/render/texture/VKSimpleTexture.h>
#include <vulkan/render/texture/VKTextureView.h>
#include <vulkan/util/VulkanConversions.h>

namespace neon::vulkan
{
    void VKSimpleFrameBuffer::initRenderPass(const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                                             const std::optional<FrameBufferDepthCreateInfo>& depthInfo)
    {
        namespace vc = conversions;
        SamplesPerTexel depthSamples = depthInfo.has_value() ? depthInfo->samples : SamplesPerTexel::COUNT_1;

        _renderPass = std::make_unique<VKRenderPass>(
            getApplication()->getApplication(), vc::vkFormat(textureInfos),
            conversions::vkSampleCountFlagBits(textureInfos), depthInfo.has_value(), false,
            depthSamples != SamplesPerTexel::COUNT_1, getApplication()->getDepthImageFormat(),
            conversions::vkSampleCountFlagBits(depthSamples));
    }

    void VKSimpleFrameBuffer::initOutputs(const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                                          const std::optional<FrameBufferDepthCreateInfo>& depthInfo)
    {
        _outputs.reserve(textureInfos.size());
        for (auto& createInfo : textureInfos) {
            _outputs.emplace_back(createInfo.name, createInfo, nullptr, nullptr);
        }

        if (depthInfo.has_value()) {
            _depth = SimpleFrameBufferDepth(depthInfo->name, *depthInfo, nullptr);
        }
    }

    void VKSimpleFrameBuffer::createImages()
    {
        auto* app = getApplication()->getApplication();
        ImageCreateInfo info;
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
            info.samples = output.createInfo.samples;

            auto texture = std::make_shared<VKSimpleTexture>(app, name, info, nullptr);
            output.texture = std::make_shared<VKTextureView>(app, name, ImageViewCreateInfo(), texture);

            if (output.createInfo.samples != SamplesPerTexel::COUNT_1) {
                info.samples = SamplesPerTexel::COUNT_1;
                auto resolved = std::make_shared<VKSimpleTexture>(app, name, info, nullptr);
                output.resolved = std::make_shared<VKTextureView>(
                    app, name, ImageViewCreateInfo{.aspect = {ViewAspect::COLOR}}, resolved);
            } else {
                output.resolved = output.texture;
            }
        }

        if (_depth) {
            auto name = _depth->name.value_or("");
            ImageViewCreateInfo viewCreateInfo{
                .aspect = {ViewAspect::DEPTH, ViewAspect::STENCIL}
            };

            info.samples = _depth->createInfo.samples;
            info.usages = {TextureUsage::DEPTH_STENCIL_ATTACHMENT, TextureUsage::SAMPLING};

            auto texture = std::make_shared<VKSimpleTexture>(app, name, info, nullptr);
            auto view = std::make_shared<VKTextureView>(app, name, viewCreateInfo, texture);
            _depth->texture = view;
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
            views.push_back(item.texture->vk());
            if (item.resolved != item.texture) {
                views.push_back(item.resolved->vk());
            }
        }

        if (_depth) {
            views.push_back(_depth->texture->vk());
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
        vkDestroyFramebuffer(getApplication()->getDevice()->getRaw(), _frameBuffer, nullptr);
    }

    VKSimpleFrameBuffer::VKSimpleFrameBuffer(Application* application, rush::Vec2ui dimensions,
                                             const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                                             const std::optional<FrameBufferDepthCreateInfo>& depthInfo) :
        VKFrameBuffer(application),
        _dimensions(dimensions)
    {
        initRenderPass(textureInfos, depthInfo);
        initOutputs(textureInfos, depthInfo);
        createImages();
        createFrameBuffer();
    }

    void VKSimpleFrameBuffer::recreate(rush::Vec2ui dimensions)
    {
        _dimensions = std::move(dimensions);
        destroyFrameBuffer();
        createImages();
        createFrameBuffer();
    }
} // namespace neon::vulkan