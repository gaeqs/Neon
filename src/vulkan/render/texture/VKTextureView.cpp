//
// Created by gaeqs on 28/05/2025.
//

#include "VKTextureView.h"

#include <vulkan/util/VKUtil.h>

namespace neon::vulkan
{

    VKTextureView::VKTextureView(Application* application, std::string name, const ImageViewCreateInfo& info,
                                 std::shared_ptr<Texture> texture) :
        VKResource(application),
        TextureView(std::move(name), info, std::move(texture))
    {
        VkImage native = static_cast<VkImage>(getTexture()->getNativeHandle());
        VkFormat format;
        if (info.format) {
            format = vc::vkFormat(info.format.value());
        } else {
            format = vc::vkFormat(texture->getFormat());
        }

        _view = vulkan_util::createImageView(getApplication()->getDevice()->getRaw(), native, format,
                                             VK_IMAGE_ASPECT_COLOR_BIT, info);
    }

    VKTextureView::~VKTextureView()
    {
        auto device = getApplication()->getDevice()->getRaw();
        getApplication()->getBin()->destroyLater(device, getRuns(), _view, vkDestroyImageView);
    }

    void* VKTextureView::getNativeHandle()
    {
        return _view;
    }

    const void* VKTextureView::getNativeHandle() const
    {
        return _view;
    }

} // namespace neon::vulkan