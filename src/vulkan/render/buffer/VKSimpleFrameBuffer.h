//
// Created by gaeqs on 29/05/25.
//

#ifndef VKSIMPLEFRAMEBUFFER_H
#define VKSIMPLEFRAMEBUFFER_H

#include <neon/render/buffer/FrameBufferTextureCreateInfo.h>
#include <neon/render/texture/TextureView.h>
#include <neon/structure/MutableAsset.h>

#include <vulkan/render/VKFrameBuffer.h>

#include <vulkan/vulkan.h>
#include <vulkan/render/VKRenderPass.h>

namespace neon::vulkan
{

    struct SimpleFrameBufferOutput
    {
        std::optional<std::string> name;
        FrameBufferTextureCreateInfo createInfo;
        std::shared_ptr<MutableAsset<TextureView>> texture;
        std::shared_ptr<MutableAsset<TextureView>> resolved;
    };

    struct SimpleFrameBufferDepth
    {
        std::optional<std::string> name;
        FrameBufferDepthCreateInfo createInfo;
        std::shared_ptr<MutableAsset<TextureView>> texture;
    };

    class VKSimpleFrameBuffer : public VKFrameBuffer
    {
        SamplesPerTexel _samplesPerTexel;
        VkFramebuffer _frameBuffer;
        std::unique_ptr<VKRenderPass> _renderPass;
        std::vector<SimpleFrameBufferOutput> _outputs;
        std::optional<SimpleFrameBufferDepth> _depth;
        rush::Vec2ui _dimensions;

        void initRenderPass(const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                            const std::optional<FrameBufferDepthCreateInfo>& depthInfo);

        void initOutputs(const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                         const std::optional<FrameBufferDepthCreateInfo>& depthInfo);

        void createImages();

        void createFrameBuffer();

        void destroyFrameBuffer();

      public:
        VKSimpleFrameBuffer(Application* application, rush::Vec2ui dimensions, SamplesPerTexel samples,
                            const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                            const std::optional<FrameBufferDepthCreateInfo>& depthInfo);

        ~VKSimpleFrameBuffer() override;

        [[nodiscard]] rush::Vec2ui getDimensions() const override;

        [[nodiscard]] bool hasDepth() const override;

        [[nodiscard]] uint32_t getColorAttachmentAmount() const override;

        [[nodiscard]] std::vector<VkFormat> getColorFormats() const override;

        [[nodiscard]] std::optional<VkFormat> getDepthFormat() const override;

        [[nodiscard]] const VKRenderPass& getRenderPass() const override;

        [[nodiscard]] VKRenderPass& getRenderPass() override;

        [[nodiscard]] SamplesPerTexel getSamples() const override;

        [[nodiscard]] bool renderImGui() override;

        [[nodiscard]] std::vector<FrameBufferOutput> getOutputs() const override;

        [[nodiscard]] void* getNativeHandle() override;

        [[nodiscard]] const void* getNativeHandle() const override;

        void recreate(rush::Vec2ui dimensions);

        [[nodiscard]] VkFramebuffer vk() const;
    };
} // namespace neon::vulkan

#endif // VKSIMPLEFRAMEBUFFER_H
