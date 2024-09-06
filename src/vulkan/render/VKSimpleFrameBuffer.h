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

namespace neon {
    class Application;

    class Texture;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKSimpleFrameBuffer : public VKFrameBuffer {

        AbstractVKApplication* _vkApplication;

        VkFramebuffer _frameBuffer;

        std::vector<VkImage> _images;
        std::vector<VkDeviceMemory> _memories;
        std::vector<VkImageView> _imageViews;
        std::vector<VkImageLayout> _layouts;
        std::vector<VkDescriptorSet> _imGuiDescriptors;
        std::vector<std::shared_ptr<Texture>> _textures;

        std::vector<FrameBufferTextureCreateInfo> _createInfos;
        VkExtent2D _extent;

        VKRenderPass _renderPass;

        bool _depth;

        void createImages(std::shared_ptr<Texture> overrideDepth);

        void createFrameBuffer();

        void cleanup();

        void cleanupImages();

    public:

        VKSimpleFrameBuffer(Application* application,
                            const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                            std::pair<uint32_t, uint32_t> extent,
                            bool depth);

        VKSimpleFrameBuffer(Application* application,
                            const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                            std::pair<uint32_t, uint32_t> extent,
                            std::shared_ptr<Texture> depthTexture);

        ~VKSimpleFrameBuffer() override;

        [[nodiscard]] VkFramebuffer getRaw() const override;

        [[nodiscard]] bool hasDepth() const override;

        [[nodiscard]] uint32_t getColorAttachmentAmount() const override;

        [[nodiscard]] std::vector<VkFormat> getColorFormats() const override;

        [[nodiscard]] VkFormat getDepthFormat() const override;

        [[nodiscard]] VKRenderPass const& getRenderPass() const override;

        [[nodiscard]] VKRenderPass& getRenderPass() override;

        [[nodiscard]] uint32_t getWidth() const override;

        [[nodiscard]] uint32_t getHeight() const override;

        [[nodiscard]] ImTextureID getImGuiDescriptor(uint32_t index);

        bool renderImGui() override;

        [[nodiscard]] const std::vector<std::shared_ptr<Texture>>&
        getTextures() const;

        void recreate(std::pair<uint32_t, uint32_t> size);

        [[nodiscard]] bool defaultRecreationCondition() const;

        [[nodiscard]] std::pair<uint32_t, uint32_t>
        defaultRecreationParameters() const;

    };
}


#endif //NEON_VKSIMPLEFRAMEBUFFER_H
