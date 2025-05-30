//
// Created by gaeqs on 29/05/25.
//

#include "VKSimpleFrameBuffer.h"

#include <vulkan/render/texture/VKSimpleTexture.h>
#include <vulkan/render/texture/VKTextureView.h>

namespace neon::vulkan
{
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
            std::shared_ptr<TextureView> view =
                std::make_shared<VKTextureView>(app, name, ImageViewCreateInfo(), texture);
            output.texture = view;

            if (output.createInfo.samples != SamplesPerTexel::COUNT_1) {
                info.samples = SamplesPerTexel::COUNT_1;
                auto resolved = std::make_shared<VKSimpleTexture>(app, name, info, nullptr);
                std::shared_ptr<TextureView> resolvedView = std::make_shared<VKTextureView>(
                    app, name, ImageViewCreateInfo{.aspect = {ViewAspect::COLOR}}, resolved);
                output.resolved = resolvedView;
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
            std::shared_ptr<TextureView> view = std::make_shared<VKTextureView>(app, name, viewCreateInfo, texture);
            _depth->texture = view;
        }
    }

    VKSimpleFrameBuffer::VKSimpleFrameBuffer(Application* application, rush::Vec2ui dimensions,
                                             const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                                             const std::optional<FrameBufferDepthCreateInfo>& depthInfo) :
        VKFrameBuffer(application),
        _dimensions(dimensions)
    {
        initOutputs(textureInfos, depthInfo);
    }
} // namespace neon::vulkan