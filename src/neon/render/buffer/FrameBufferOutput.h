//
// Created by gaeqs on 06/02/2025.
//

#ifndef FRAMEBUFFEROUTPUT_H
#define FRAMEBUFFEROUTPUT_H

#include <memory>
#include <neon/render/texture/TextureView.h>
#include <neon/structure/MutableAsset.h>

namespace neon
{
    enum class FrameBufferOutputType
    {
        COLOR,
        DEPTH,
        SWAP
    };

    struct FrameBufferOutput
    {
        FrameBufferOutputType type;
        std::shared_ptr<MutableAsset<TextureView>> texture;
        std::shared_ptr<MutableAsset<TextureView>> resolvedTexture;
    };
} // namespace neon

#endif // FRAMEBUFFEROUTPUT_H
