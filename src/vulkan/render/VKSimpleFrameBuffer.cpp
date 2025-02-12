//
// Created by grial on 15/12/22.
//

#include "VKSimpleFrameBuffer.h"

#include <stdexcept>
#include <utility>

#include <imgui_impl_vulkan.h>
#include <neon/render/buffer/FrameBuffer.h>

#include <neon/structure/Room.h>
#include <neon/render/texture/Texture.h>
#include <vulkan/util/VKUtil.h>
#include <vulkan/util/VulkanConversions.h>
#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan {
    void VKSimpleFrameBuffer::createImages(
        std::shared_ptr<Texture> overrideDepth) {
        _images.clear();

        ImageCreateInfo info;
        info.width = _extent.width;
        info.height = _extent.height;
        info.depth = 1;
        info.mipmaps = 1;
        info.layers = 1;
        info.usages = {TextureUsage::COLOR_ATTACHMENT, TextureUsage::SAMPLING, TextureUsage::TRANSFER_SOURCE};

        for (const auto& createInfo: _createInfos) {
            info.format = createInfo.format;
            info.layers = createInfo.layers;
            info.samples = createInfo.samples;
            auto [image, memory] = vulkan_util::createImage(
                _vkApplication->getDevice()->getRaw(),
                _vkApplication->getPhysicalDevice().getRaw(),
                info,
                createInfo.imageView.viewType
            );

            auto view = vulkan_util::createImageView(
                _vkApplication->getDevice()->getRaw(),
                image,
                conversions::vkFormat(info.format),
                VK_IMAGE_ASPECT_COLOR_BIT,
                createInfo.imageView
            );

            SimpleFrameBufferSlot slot;
            slot.image = SimpleFrameBufferImage(
                createInfo.name,
                false,
                info,
                createInfo.sampler,
                image,
                memory,
                _hasMultisampling ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                view,
                nullptr,
                nullptr
            );

            if (_hasMultisampling) {
                // Create resolve texture
                info.samples = SamplesPerTexel::COUNT_1;
                auto [resolveImage, resolveMemory] = vulkan_util::createImage(
                    _vkApplication->getDevice()->getRaw(),
                    _vkApplication->getPhysicalDevice().getRaw(),
                    info,
                    createInfo.imageView.viewType
                );

                auto resolveView = vulkan_util::createImageView(
                    _vkApplication->getDevice()->getRaw(),
                    resolveImage,
                    conversions::vkFormat(info.format),
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    createInfo.imageView
                );

                slot.resolveImage = SimpleFrameBufferImage(
                    createInfo.resolveName,
                    false,
                    info,
                    createInfo.sampler,
                    resolveImage,
                    resolveMemory,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    resolveView,
                    nullptr,
                    nullptr
                );
            }

            _images.push_back(slot);
        }

        // Add depth texture
        if (!_depth) return;

        SamplerCreateInfo depthSampler;
        depthSampler.anisotropy = false;
        depthSampler.minificationFilter = TextureFilter::NEAREST;
        depthSampler.magnificationFilter = TextureFilter::NEAREST;

        if (overrideDepth == nullptr) {
            info.samples = _depthSamples;
            info.usages = {
                TextureUsage::DEPTH_STENCIL_ATTACHMENT,
                TextureUsage::SAMPLING
            };
            auto [image, memory] = vulkan_util::createImage(
                _vkApplication->getDevice()->getRaw(),
                _vkApplication->getPhysicalDevice().getRaw(),
                info,
                TextureViewType::NORMAL_2D,
                _vkApplication->getDepthImageFormat() // Overrides info's format.
            );

            auto view = vulkan_util::createImageView(
                _vkApplication->getDevice()->getRaw(),
                image,
                _vkApplication->getDepthImageFormat(),
                VK_IMAGE_ASPECT_DEPTH_BIT,
                ImageViewCreateInfo()
            );

            SimpleFrameBufferSlot slot{
                {
                    _depthName,
                    true,
                    info,
                    depthSampler,
                    image,
                    memory,
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                    view,
                    nullptr,
                    nullptr
                }
            };

            _images.push_back(slot);
        } else {
            auto& impl = overrideDepth->getImplementation();

            SimpleFrameBufferSlot slot{
                {
                    _depthName,
                    true,
                    info,
                    depthSampler,
                    impl.getImage(),
                    impl.getMemory(),
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                    impl.getImageView(),
                    nullptr,
                    overrideDepth
                }
            };

            _images.push_back(slot);
        }
    }

    void VKSimpleFrameBuffer::createFrameBuffer() {
        uint32_t layers = 1;
        if (!_createInfos.empty()) {
            layers = _createInfos[0].layers;
        }
        for (const auto& item: _createInfos) {
            layers = std::min(layers, item.layers);
        }

        std::vector<VkImageView> views;
        views.reserve(_images.size() * 2);

        for (auto& item: _images) {
            views.push_back(item.image.view);
            if (item.resolveImage.has_value()) {
                views.push_back(item.resolveImage->view);
            }
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass.getRaw();
        framebufferInfo.attachmentCount = views.size();
        framebufferInfo.pAttachments = views.data();
        framebufferInfo.width = _extent.width;
        framebufferInfo.height = _extent.height;
        framebufferInfo.layers = layers;

        if (vkCreateFramebuffer(
                _vkApplication->getDevice()->getRaw(),
                &framebufferInfo,
                nullptr,
                &_frameBuffer
            ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }

    void VKSimpleFrameBuffer::createOutputs(Application* application) {
        uint32_t i = 0;
        for (auto& item: _images) {
            ++i;

            FrameBufferOutput output;
            output.type = item.image.depth ? FrameBufferOutputType::DEPTH : FrameBufferOutputType::COLOR;

            if (item.image.overrideTexture != nullptr) {
                output.texture = item.image.overrideTexture;
            } else {
                output.texture = std::make_shared<Texture>(
                    application,
                    item.image.name.value_or(std::to_string(i)),
                    item.image.image,
                    item.image.memory,
                    item.image.view,
                    item.image.layout,
                    static_cast<int32_t>(_extent.width),
                    static_cast<int32_t>(_extent.height),
                    1,
                    item.image.info.format,
                    item.image.info.samples,
                    item.image.sampler
                );
            }

            if (item.resolveImage.has_value()) {
                if (item.resolveImage->overrideTexture != nullptr) {
                    output.resolvedTexture = item.image.overrideTexture;
                } else {
                    output.resolvedTexture = std::make_shared<Texture>(
                        application,
                        item.resolveImage->name.value_or(std::to_string(i)),
                        item.resolveImage->image,
                        item.resolveImage->memory,
                        item.resolveImage->view,
                        item.resolveImage->layout,
                        static_cast<int32_t>(_extent.width),
                        static_cast<int32_t>(_extent.height),
                        1,
                        item.resolveImage->info.format,
                        item.resolveImage->info.samples,
                        item.resolveImage->sampler
                    );
                }
            } else {
                output.resolvedTexture = output.texture;
            }

            _outputs.push_back(output);
        }
    }

    void VKSimpleFrameBuffer::cleanup() {
        auto d = _vkApplication->getDevice()->getRaw();

        for (auto& slot: _images) {
            if (slot.image.imGuiDescriptor != nullptr) {
                ImGui_ImplVulkan_RemoveTexture(slot.image.imGuiDescriptor);
                slot.image.imGuiDescriptor = nullptr;
            }
            if (slot.resolveImage.has_value() && slot.resolveImage->imGuiDescriptor != nullptr) {
                ImGui_ImplVulkan_RemoveTexture(slot.resolveImage->imGuiDescriptor);
                slot.resolveImage->imGuiDescriptor = nullptr;
            }
        }
        vkDestroyFramebuffer(d, _frameBuffer, nullptr);
    }

    void VKSimpleFrameBuffer::cleanupImages() {
        auto d = _vkApplication->getDevice()->getRaw();
        for (auto& slot: _images) {
            vkDestroyImageView(d, slot.image.view, nullptr);
            vkDestroyImage(d, slot.image.image, nullptr);
            vkFreeMemory(d, slot.image.memory, nullptr);
            if (slot.resolveImage.has_value()) {
                vkDestroyImageView(d, slot.resolveImage->view, nullptr);
                vkDestroyImage(d, slot.resolveImage->image, nullptr);
                vkFreeMemory(d, slot.resolveImage->memory, nullptr);
            }
        }
    }

    VKSimpleFrameBuffer::VKSimpleFrameBuffer(
        Application* application,
        const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
        std::pair<uint32_t, uint32_t> extent,
        bool depth,
        std::optional<std::string> depthName,
        SamplesPerTexel depthSamples)
        : VKFrameBuffer(),
          _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _frameBuffer(VK_NULL_HANDLE),
          _createInfos(textureInfos),
          _extent({extent.first, extent.second}),
          _renderPass(application,
                      conversions::vkFormat(textureInfos),
                      conversions::vkSampleCountFlagBits(textureInfos),
                      depth,
                      false,
                      depthSamples != SamplesPerTexel::COUNT_1,
                      _vkApplication->getDepthImageFormat(),
                      conversions::vkSampleCountFlagBits(depthSamples)),
          _depth(depth),
          _depthName(std::move(depthName)),
          _depthSamples(depthSamples),
          _hasMultisampling(depthSamples != SamplesPerTexel::COUNT_1),
          _colorOutputs(textureInfos.size()) {
        createImages(nullptr);
        createFrameBuffer();
        createOutputs(application);
    }

    VKSimpleFrameBuffer::VKSimpleFrameBuffer(
        Application* application,
        const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
        std::pair<uint32_t, uint32_t> extent,
        std::shared_ptr<Texture> depthTexture) :
        VKFrameBuffer(),
        _vkApplication(dynamic_cast<AbstractVKApplication*>(
            application->getImplementation())),
        _frameBuffer(VK_NULL_HANDLE),
        _createInfos(textureInfos),
        _extent({extent.first, extent.second}),
        _renderPass(application,
                    conversions::vkFormat(textureInfos),
                    conversions::vkSampleCountFlagBits(textureInfos),
                    depthTexture != nullptr,
                    false,
                    true,
                    conversions::vkFormat(depthTexture->getFormat()),
                    conversions::vkSampleCountFlagBits(depthTexture->getSamples())),
        _depth(depthTexture != nullptr),
        _depthName(depthTexture == nullptr ? nullptr : depthTexture->getName()),
        _depthSamples(depthTexture != nullptr ? depthTexture->getSamples() : SamplesPerTexel::COUNT_1),
        _colorOutputs(textureInfos.size()) {
        createImages(depthTexture);
        createFrameBuffer();
        createOutputs(application);
    }

    VKSimpleFrameBuffer::~VKSimpleFrameBuffer() {
        cleanup();

        // DO NOT cleanup images. Images will be destroyed automatically
        // when their respective textures are deleted.
        // This allows textures to be used externally even if
        // the frame buffer is destroyed.
        for (const auto& output: _outputs) {
            output.texture->getImplementation().makeInternal();
            if (output.texture != output.resolvedTexture) {
                output.resolvedTexture->getImplementation().makeInternal();
            }
        }
    }

    VkFramebuffer VKSimpleFrameBuffer::getRaw() const {
        return _frameBuffer;
    }

    bool VKSimpleFrameBuffer::hasDepth() const {
        return _depth;
    }

    uint32_t VKSimpleFrameBuffer::getColorAttachmentAmount() const {
        return _colorOutputs;
    }

    void VKSimpleFrameBuffer::recreate(std::pair<uint32_t, uint32_t> size) {
        _extent = {size.first, size.second};

        cleanup();
        cleanupImages();
        createImages(nullptr);
        createFrameBuffer();

        // Refresh the textures
        for (uint32_t i = 0; i < _images.size(); ++i) {
            auto& item = _images[i];
            auto& texture = _outputs[i].texture;
            texture->getImplementation().changeExternalImageView(
                static_cast<int32_t>(_extent.width),
                static_cast<int32_t>(_extent.height),
                item.image.image,
                item.image.memory,
                item.image.view
            );

            if (auto& resolved = _outputs[i].resolvedTexture; resolved != texture) {
                resolved->getImplementation().changeExternalImageView(
                    static_cast<int32_t>(_extent.width),
                    static_cast<int32_t>(_extent.height),
                    item.resolveImage->image,
                    item.resolveImage->memory,
                    item.resolveImage->view
                );
            }
        }
    }

    std::vector<VkFormat> VKSimpleFrameBuffer::getColorFormats() const {
        return conversions::vkFormat(_createInfos);
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

    std::vector<FrameBufferOutput> VKSimpleFrameBuffer::getOutputs() const {
        return _outputs;
    }

    bool VKSimpleFrameBuffer::renderImGui() {
        return false;
    }

    ImTextureID VKSimpleFrameBuffer::getImGuiDescriptor(uint32_t index) {
        auto& image = _images[index];
        auto& target = image.resolveImage.has_value() ? image.resolveImage.value() : image.image;

        if (target.imGuiDescriptor == nullptr) {
            auto& texture = _outputs[index].resolvedTexture;
            target.imGuiDescriptor = ImGui_ImplVulkan_AddTexture(
                texture->getImplementation().getSampler(),
                texture->getImplementation().getImageView(),
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );
        }

        return target.imGuiDescriptor;
    }

    uint32_t VKSimpleFrameBuffer::getWidth() const {
        return _extent.width;
    }

    uint32_t VKSimpleFrameBuffer::getHeight() const {
        return _extent.height;
    }

    SamplesPerTexel VKSimpleFrameBuffer::getSamples() const {
        return _depthSamples;
    }

    bool VKSimpleFrameBuffer::defaultRecreationCondition() const {
        auto extent = _vkApplication->getSwapChainExtent();
        if (extent.width == 0 || extent.height == 0) return false;
        return extent.width != getWidth() || extent.height != getHeight();
    }

    std::pair<uint32_t, uint32_t>
    VKSimpleFrameBuffer::defaultRecreationParameters() const {
        auto extent = _vkApplication->getSwapChainExtent();
        return {extent.width, extent.height};
    }
}
