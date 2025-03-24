//
// Created by gaeqs on 06/02/2025.
//

#ifndef FRAMEBUFFEROUTPUT_H
#define FRAMEBUFFEROUTPUT_H

#include <memory>
#include <neon/render/texture/Texture.h>

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
        std::shared_ptr<Texture> texture;
        std::shared_ptr<Texture> resolvedTexture;
    };
} // namespace neon

#endif //FRAMEBUFFEROUTPUT_H
