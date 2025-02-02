//
//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_VKFRAMEBUFFER_H
#define NEON_VKFRAMEBUFFER_H

#include <vulkan/vulkan.h>

#include <vector>
#include <neon/render/texture/TextureCreateInfo.h>

namespace neon::vulkan {
    class VKRenderPass;

    class VKFrameBuffer {

    public:

        VKFrameBuffer(const VKFrameBuffer& other) = delete;

        VKFrameBuffer() = default;

        virtual ~VKFrameBuffer() = default;

        [[nodiscard]] virtual bool hasDepth() const = 0;

        [[nodiscard]] virtual uint32_t getColorAttachmentAmount() const = 0;

        [[nodiscard]] virtual VkFramebuffer getRaw() const = 0;

        [[nodiscard]] virtual std::vector<VkFormat> getColorFormats() const = 0;

        [[nodiscard]] virtual VkFormat getDepthFormat() const = 0;

        [[nodiscard]] virtual const VKRenderPass& getRenderPass() const = 0;

        [[nodiscard]] virtual VKRenderPass& getRenderPass() = 0;

        [[nodiscard]] virtual uint32_t getWidth() const = 0;

        [[nodiscard]] virtual uint32_t getHeight() const = 0;

        [[nodiscard]] virtual SamplesPerTexel getSamples() const = 0;

        [[nodiscard]] virtual bool renderImGui() = 0;
    };
}

#endif //NEON_VKFRAMEBUFFER_H
