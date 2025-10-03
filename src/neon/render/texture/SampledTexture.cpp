//
// Created by gaeqs on 28/05/2025.
//

#include "SampledTexture.h"
#ifdef USE_VULKAN
    #include <vulkan/render/texture/VKSampledTexture.h>
#endif
namespace neon
{

    SampledTexture::SampledTexture(std::string name, std::shared_ptr<MutableAsset<TextureView>> view,
                                   std::shared_ptr<Sampler> sampler) :
        Asset(typeid(SampledTexture), std::move(name)),
        _view(std::move(view)),
        _sampler(std::move(sampler))
    {
    }

    const std::shared_ptr<MutableAsset<TextureView>>& SampledTexture::getView() const
    {
        return _view;
    }

    uint64_t SampledTexture::getViewVersion() const
    {
        return _view->getVersion();
    }

    const std::shared_ptr<Sampler>& SampledTexture::getSampler() const
    {
        return _sampler;
    }

    std::tuple<void*, void*, std::any> SampledTexture::getNativeHandlers()
    {
        auto& v = _view->get();
        return {v->getNativeHandle(), _sampler->getNativeHandle(), v->getTexture()->getLayoutNativeHandle()};
    }

    std::tuple<const void*, const void*, std::any> SampledTexture::getNativeHandlers() const
    {
        auto& v = _view->get();
        return {v->getNativeHandle(), _sampler->getNativeHandle(), v->getTexture()->getLayoutNativeHandle()};
    }

    std::shared_ptr<SampledTexture> SampledTexture::create(std::string name, std::shared_ptr<TextureView> view,
                                                           std::shared_ptr<Sampler> sampler)
    {
#ifdef USE_VULKAN
        auto mut = std::make_shared<MutableAsset<TextureView>>(std::move(view));
        return std::make_shared<vulkan::VKSampledTexture>(std::move(name), std::move(mut), std::move(sampler));
#else
        return nullptr;
#endif
    }

    std::shared_ptr<SampledTexture> SampledTexture::create(std::string name,
                                                           std::shared_ptr<MutableAsset<TextureView>> view,
                                                           std::shared_ptr<Sampler> sampler)
    {
#ifdef USE_VULKAN
        return std::make_shared<vulkan::VKSampledTexture>(std::move(name), std::move(view), std::move(sampler));
#else
        return nullptr;
#endif
    }

    std::shared_ptr<SampledTexture> SampledTexture::create(Application* application, std::string name,
                                                           std::shared_ptr<Texture> texture)
    {
        if (texture == nullptr) {
            return nullptr;
        }
        auto sampler = Sampler::create(application, name, SamplerCreateInfo());
        auto view = TextureView::create(application, name, TextureViewCreateInfo(), std::move(texture));
        return create(std::move(name), std::move(view), std::move(sampler));
    }

    std::shared_ptr<SampledTexture> SampledTexture::create(Application* application, std::shared_ptr<Texture> texture)
    {
        std::string name = texture->getName();
        return create(application, std::move(name), std::move(texture));
    }

    std::shared_ptr<SampledTexture> SampledTexture::create(Application* application, std::string name,
                                                           std::shared_ptr<MutableAsset<TextureView>> view)
    {
        if (view == nullptr) {
            return nullptr;
        }
        auto sampler = Sampler::create(application, name, SamplerCreateInfo());
        return create(std::move(name), std::move(view), std::move(sampler));
    }

    std::shared_ptr<SampledTexture> SampledTexture::create(Application* application,
                                                           std::shared_ptr<MutableAsset<TextureView>> view)
    {
        std::string name = view->get()->getName();
        return create(application, std::string(name), std::move(view));
    }

    std::vector<std::shared_ptr<SampledTexture>> SampledTexture::create(
        Application* application, const std::vector<std::shared_ptr<MutableAsset<TextureView>>>& textures)
    {
        std::vector<std::shared_ptr<SampledTexture>> result;
        result.reserve(textures.size());
        for (auto& texture : textures) {
            result.push_back(create(application, texture));
        }
        return result;
    }

    std::vector<std::shared_ptr<SampledTexture>> SampledTexture::create(
        Application* application, const std::vector<std::shared_ptr<Texture>>& textures)
    {
        std::vector<std::shared_ptr<SampledTexture>> result;
        result.reserve(textures.size());
        for (auto& texture : textures) {
            result.push_back(create(application, texture));
        }
        return result;
    }
} // namespace neon