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
#include <vulkan/render/texture/VKTextureView.h>

namespace neon::vulkan
{

    struct SimpleFrameBufferOutput
    {
        std::optional<std::string> name;
        FrameBufferTextureCreateInfo createInfo;
        std::shared_ptr<VKTextureView> texture;
        std::shared_ptr<VKTextureView> resolved;
    };

    struct SimpleFrameBufferDepth
    {
        std::optional<std::string> name;
        FrameBufferDepthCreateInfo createInfo;
        std::shared_ptr<VKTextureView> texture;
    };

    class VKSimpleFrameBuffer : public VKFrameBuffer
    {
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
        VKSimpleFrameBuffer(Application* application, rush::Vec2ui dimensions,
                            const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                            const std::optional<FrameBufferDepthCreateInfo>& depthInfo);

        void recreate(rush::Vec2ui dimensions);
    };
} // namespace neon::vulkan

#endif // VKSIMPLEFRAMEBUFFER_H
