//
// Created by gaelr on 28/05/2023.
//

#ifndef NEON_FRAMEBUFFERTEXTURECREATEINFO_H
#define NEON_FRAMEBUFFERTEXTURECREATEINFO_H

#include <optional>
#include <string>
#include <neon/render/texture/TextureCreateInfo.h>

namespace neon
{
    struct FrameBufferTextureCreateInfo
    {
        /**
         * The name of the texture.
         */
        std::optional<std::string> name = {};

        /**
         * The name of the resolved texture.
         */
        std::optional<std::string> resolveName = {};

        /**
         * The format of the texture.
         */
        TextureFormat format = TextureFormat::R8G8B8A8;

        /**
         * The number of texture's layers.
         */
        uint32_t layers = 1;

        /**
         * The creation information of the image view.
         */
        ImageViewCreateInfo imageView = ImageViewCreateInfo();

        /**
         * The creation information of the sampler.
         */
        SamplerCreateInfo sampler = SamplerCreateInfo();

        FrameBufferTextureCreateInfo()
        {
            sampler.anisotropy = false;
            sampler.minificationFilter = TextureFilter::NEAREST;
            sampler.magnificationFilter = TextureFilter::NEAREST;
        }

        FrameBufferTextureCreateInfo(TextureFormat format_) :
            format(format_)
        {
            sampler.anisotropy = false;
            sampler.minificationFilter = TextureFilter::NEAREST;
            sampler.magnificationFilter = TextureFilter::NEAREST;
        }
    };

    struct FrameBufferDepthCreateInfo
    {
        /**
         * The name of the texture.
         */
        std::optional<std::string> name = {};
    };
} // namespace neon

#endif // NEON_FRAMEBUFFERTEXTURECREATEINFO_H
