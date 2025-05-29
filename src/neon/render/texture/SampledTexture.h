//
// Created by gaeqs on 28/05/2025.
//

#ifndef NEON_SAMPLEDTEXTURE_H
#define NEON_SAMPLEDTEXTURE_H

#include "Sampler.h"
#include "TextureView.h"

#include <memory>

namespace neon
{
    class SampledTexture
    {
        std::shared_ptr<TextureView> _view;
        std::shared_ptr<Sampler> _sampler;

      public:
        SampledTexture(std::shared_ptr<TextureView> view, std::shared_ptr<Sampler> sampler);

        virtual ~SampledTexture() = default;

        [[nodiscard]] const std::shared_ptr<TextureView>& getView() const;

        [[nodiscard]] const std::shared_ptr<Sampler>& getSampler() const;

        [[nodiscard]] virtual ImTextureID getImGuiDescriptor() const = 0;

        static std::shared_ptr<SampledTexture> create(std::shared_ptr<TextureView> view,
                                                      std::shared_ptr<Sampler> sampler);

        static std::shared_ptr<SampledTexture> create(Application* application, std::shared_ptr<Texture> texture);
    };
} // namespace neon

#endif // IMGUITEXTUREMANAGER_H
