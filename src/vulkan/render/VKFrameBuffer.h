//
//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_VKFRAMEBUFFER_H
#define NEON_VKFRAMEBUFFER_H

#include <vulkan/vulkan.h>

#include <vector>
#include <neon/render/buffer/FrameBufferOutput.h>
#include <vulkan/VKResource.h>

namespace neon::vulkan
{
    class VKRenderPass;

    class VKFrameBuffer : public VKResource
    {
      public:
        VKFrameBuffer(const VKFrameBuffer& other) = delete;

        explicit VKFrameBuffer(Application* application);

        virtual ~VKFrameBuffer() = default;

        [[nodiscard]] virtual rush::Vec2ui getDimensions() const = 0;

        [[nodiscard]] virtual bool hasDepth() const = 0;

        [[nodiscard]] virtual uint32_t getColorAttachmentAmount() const = 0;

        [[nodiscard]] virtual std::vector<VkFormat> getColorFormats() const = 0;

        [[nodiscard]] virtual std::optional<VkFormat> getDepthFormat() const = 0;

        [[nodiscard]] virtual const VKRenderPass& getRenderPass() const = 0;

        [[nodiscard]] virtual VKRenderPass& getRenderPass() = 0;

        [[nodiscard]] virtual SamplesPerTexel getSamples() const = 0;

        [[nodiscard]] virtual bool renderImGui() = 0;

        [[nodiscard]] virtual std::vector<FrameBufferOutput> getOutputs() const = 0;

        [[nodiscard]] virtual void* getNativeHandle() = 0;

        [[nodiscard]] virtual const void* getNativeHandle() const = 0;
    };
} // namespace neon::vulkan

#endif // NEON_VKFRAMEBUFFER_H
