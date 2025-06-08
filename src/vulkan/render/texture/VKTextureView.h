//
// Created by gaeqs on 28/05/2025.
//

#ifndef VKTEXTUREVIEW_H
#define VKTEXTUREVIEW_H

#include <neon/render/texture/TextureView.h>
#include <vulkan/VKResource.h>

namespace neon::vulkan
{
    class VKTextureView : public VKResource, public TextureView
    {
        VkImageView _view;

      public:
        VKTextureView(Application* application, std::string name, const ImageViewCreateInfo& info,
            std::shared_ptr<Texture> texture);

        ~VKTextureView() override;

        [[nodiscard]] void* getNativeHandle() override;

        [[nodiscard]] const void* getNativeHandle() const override;

        [[nodiscard]] VkImageView vk() const;
    };
} // namespace neon::vulkan

#endif // VKTEXTUREVIEW_H
