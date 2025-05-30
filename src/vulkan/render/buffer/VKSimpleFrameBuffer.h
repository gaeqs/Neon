//
// Created by gaeqs on 29/05/25.
//

#ifndef VKSIMPLEFRAMEBUFFER_H
#define VKSIMPLEFRAMEBUFFER_H

#include <neon/render/buffer/FrameBufferTextureCreateInfo.h>
#include <neon/render/texture/TextureView.h>
#include <neon/structure/AssetReference.h>

#include <vulkan/render/VKFrameBuffer.h>

#include <vulkan/vulkan.h>

namespace neon::vulkan
{

    struct SimpleFrameBufferOutput
    {
        std::optional<std::string> name;
        FrameBufferTextureCreateInfo createInfo;
        AssetReference<TextureView> texture;
        AssetReference<TextureView> resolved;
    };

    struct SimpleFrameBufferDepth
    {
        std::optional<std::string> name;
        FrameBufferDepthCreateInfo createInfo;
        AssetReference<TextureView> texture;
    };

    class VKSimpleFrameBuffer : public VKFrameBuffer
    {
        VkFramebuffer _frameBuffer;
        std::vector<SimpleFrameBufferOutput> _outputs;
        std::optional<SimpleFrameBufferDepth> _depth;
        rush::Vec2ui _dimensions;

        void initOutputs(const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                         const std::optional<FrameBufferDepthCreateInfo>& depthInfo);


        void createImages();

      public:
        VKSimpleFrameBuffer(Application* application, rush::Vec2ui dimensions,
                            const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                            const std::optional<FrameBufferDepthCreateInfo>& depthInfo);
    };
} // namespace neon::vulkan

#endif // VKSIMPLEFRAMEBUFFER_H
