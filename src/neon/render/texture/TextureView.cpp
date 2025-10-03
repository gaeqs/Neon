//
// Created by gaeqs on 28/05/2025.
//

#include "TextureView.h"

#ifdef USE_VULKAN
    #include <vulkan/render/texture/VKTextureView.h>
#endif

namespace neon
{

    TextureView::TextureView(std::string name, TextureViewCreateInfo info, std::shared_ptr<Texture> texture) :
        Asset(typeid(TextureView), std::move(name)),
        _info(std::move(info)),
        _texture(std::move(texture))
    {
    }

    const std::shared_ptr<Texture>& TextureView::getTexture() const
    {
        return _texture;
    }

    const TextureViewCreateInfo& TextureView::getInfo() const
    {
        return _info;
    }

    std::shared_ptr<TextureView> TextureView::create(Application* application, std::string name,
                                                     const TextureViewCreateInfo& info, std::shared_ptr<Texture> texture)
    {
#ifdef USE_VULKAN
        return std::make_shared<vulkan::VKTextureView>(application, std::move(name), info, std::move(texture));
#else
        return nullptr;
#endif
    }
} // namespace neon