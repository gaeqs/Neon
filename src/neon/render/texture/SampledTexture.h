//
// Created by gaeqs on 28/05/2025.
//

#ifndef NEON_SAMPLEDTEXTURE_H
#define NEON_SAMPLEDTEXTURE_H

#include "Sampler.h"
#include "TextureView.h"

#include <imgui.h>
#include <memory>
#include <neon/structure/MutableAsset.h>

namespace neon
{
    class SampledTexture : public Asset
    {
        std::shared_ptr<MutableAsset<TextureView>> _view;
        std::shared_ptr<Sampler> _sampler;

      public:
        SampledTexture(std::string name, std::shared_ptr<MutableAsset<TextureView>> view,
                       std::shared_ptr<Sampler> sampler);

        ~SampledTexture() override = default;

        [[nodiscard]] const std::shared_ptr<MutableAsset<TextureView>>& getView() const;

        [[nodiscard]] uint64_t getViewVersion() const;

        [[nodiscard]] const std::shared_ptr<Sampler>& getSampler() const;

        [[nodiscard]] virtual ImTextureID getImGuiDescriptor() const = 0;

        [[nodiscard]] std::tuple<void*, void*, std::any> getNativeHandlers();

        [[nodiscard]] std::tuple<const void*, const void*, std::any> getNativeHandlers() const;

        static std::shared_ptr<SampledTexture> create(std::string name, std::shared_ptr<TextureView> view,
                                                      std::shared_ptr<Sampler> sampler);

        static std::shared_ptr<SampledTexture> create(std::string name, std::shared_ptr<MutableAsset<TextureView>> view,
                                                      std::shared_ptr<Sampler> sampler);

        static std::shared_ptr<SampledTexture> create(Application* application, std::string name,
                                                      std::shared_ptr<Texture> texture);

        static std::shared_ptr<SampledTexture> create(Application* application, std::shared_ptr<Texture> texture);

        static std::shared_ptr<SampledTexture> create(Application* application, std::string name,
                                                      std::shared_ptr<MutableAsset<TextureView>> view);

        static std::shared_ptr<SampledTexture> create(Application* application,
                                                      std::shared_ptr<MutableAsset<TextureView>> view);
    };
} // namespace neon

#endif // IMGUITEXTUREMANAGER_H
