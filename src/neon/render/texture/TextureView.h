//
// Created by gaeqs on 28/05/2025.
//

#ifndef NEON_TEXTUREVIEW_H
#define NEON_TEXTUREVIEW_H

#include <memory>

#include <neon/structure/Asset.h>
#include <neon/render/texture/Texture.h>
#include "TextureCreateInfo.h"

namespace neon
{
    class TextureView : public Asset
    {
        TextureViewCreateInfo _info;
        std::shared_ptr<Texture> _texture;

      public:
        TextureView(std::string name, TextureViewCreateInfo info, std::shared_ptr<Texture> texture);

        [[nodiscard]] const std::shared_ptr<Texture>& getTexture() const;

        [[nodiscard]] const TextureViewCreateInfo& getInfo() const;

        [[nodiscard]] virtual void* getNativeHandle() = 0;

        [[nodiscard]] virtual const void* getNativeHandle() const = 0;

        [[nodiscard]] static std::shared_ptr<TextureView> create(Application* application, std::string name,
                                                                 const TextureViewCreateInfo& info,
                                                                 std::shared_ptr<Texture> texture);
    };
} // namespace neon

#endif // NEON_TEXTUREVIEW_H
