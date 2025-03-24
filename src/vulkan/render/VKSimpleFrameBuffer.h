//
// Created by grial on 15/12/22.
//

#ifndef NEON_VKSIMPLEFRAMEBUFFER_H
#define NEON_VKSIMPLEFRAMEBUFFER_H

#include <vector>
#include <memory>

#include <neon/render/buffer/FrameBufferTextureCreateInfo.h>

#include <vulkan/render/VKFrameBuffer.h>
#include <vulkan/render/VKRenderPass.h>

#include <imgui.h>

namespace neon
{
    class Application;

    class Texture;
} // namespace neon

namespace neon::vulkan
{
    class AbstractVKApplication;

    struct SimpleFrameBufferImage
    {
        std::optional<std::string> name;
        bool depth;
        ImageCreateInfo info;
        SamplerCreateInfo sampler;
        VkImage image;
        VkDeviceMemory memory;
        VkImageLayout layout;
        VkImageView view;
        VkDescriptorSet imGuiDescriptor;
        std::shared_ptr<Texture> overrideTexture;
    };

    struct SimpleFrameBufferSlot
    {
        SimpleFrameBufferImage image;
        std::optional<SimpleFrameBufferImage> resolveImage;
    };

    class VKSimpleFrameBuffer : public VKFrameBuffer
    {
        AbstractVKApplication* _vkApplication;

        VkFramebuffer _frameBuffer;

        std::vector<FrameBufferTextureCreateInfo> _createInfos;
        std::vector<SimpleFrameBufferSlot> _images;
        std::vector<FrameBufferOutput> _outputs;

        VkExtent2D _extent;

        VKRenderPass _renderPass;

        bool _depth;
        std::optional<std::string> _depthName;
        SamplesPerTexel _depthSamples;
        bool _hasMultisampling;
        size_t _colorOutputs;

        void createImages(std::shared_ptr<Texture> overrideDepth);

        void createFrameBuffer();

        void createOutputs(Application* application);

        void cleanup();

        void cleanupImages();

      public:
        VKSimpleFrameBuffer(Application* application, const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                            std::pair<uint32_t, uint32_t> extent, bool depth, std::optional<std::string> depthName = {},
                            SamplesPerTexel depthSamples = SamplesPerTexel::COUNT_1);

        VKSimpleFrameBuffer(Application* application, const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                            std::pair<uint32_t, uint32_t> extent, std::shared_ptr<Texture> depthTexture);

        ~VKSimpleFrameBuffer() override;

        [[nodiscard]] VkFramebuffer getRaw() const override;

        [[nodiscard]] bool hasDepth() const override;

        [[nodiscard]] uint32_t getColorAttachmentAmount() const override;

        [[nodiscard]] std::vector<VkFormat> getColorFormats() const override;

        [[nodiscard]] VkFormat getDepthFormat() const override;

        [[nodiscard]] const VKRenderPass& getRenderPass() const override;

        [[nodiscard]] VKRenderPass& getRenderPass() override;

        [[nodiscard]] uint32_t getWidth() const override;

        [[nodiscard]] uint32_t getHeight() const override;

        SamplesPerTexel getSamples() const override;

        [[nodiscard]] ImTextureID getImGuiDescriptor(uint32_t index);

        bool renderImGui() override;

        [[nodiscard]] std::vector<FrameBufferOutput> getOutputs() const override;

        void recreate(std::pair<uint32_t, uint32_t> size);

        [[nodiscard]] bool defaultRecreationCondition() const;

        [[nodiscard]] std::pair<uint32_t, uint32_t> defaultRecreationParameters() const;
    };
} // namespace neon::vulkan

#endif //NEON_VKSIMPLEFRAMEBUFFER_H
