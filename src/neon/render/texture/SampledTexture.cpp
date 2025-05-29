//
// Created by gaeqs on 28/05/2025.
//

#include "SampledTexture.h"
#ifdef USE_VULKAN
    #include <vulkan/render/texture/VKSampledTexture.h>
#endif
namespace neon
{

    SampledTexture::SampledTexture(std::shared_ptr<TextureView> view, std::shared_ptr<Sampler> sampler) :
        _view(std::move(view)),
        _sampler(std::move(sampler))
    {
    }

    const std::shared_ptr<TextureView>& SampledTexture::getView() const
    {
        return _view;
    }

    const std::shared_ptr<Sampler>& SampledTexture::getSampler() const
    {
        return _sampler;
    }

    std::shared_ptr<SampledTexture> SampledTexture::create(std::shared_ptr<TextureView> view,
                                                           std::shared_ptr<Sampler> sampler)
    {
#ifdef USE_VULKAN
        return std::make_shared<vulkan::VKSampledTexture>(std::move(view), std::move(sampler));
#else
        return nullptr;
#endif
    }

    std::shared_ptr<SampledTexture> SampledTexture::create(Application* application, std::shared_ptr<Texture> texture)
    {
        if (texture == nullptr) {
            return nullptr;
        }
        auto sampler = Sampler::create(application, texture->getName(), SamplerCreateInfo());
        auto view = TextureView::create(application, texture->getName(), ImageViewCreateInfo(), std::move(texture));
        return create(std::move(view), std::move(sampler));
    }
} // namespace neon