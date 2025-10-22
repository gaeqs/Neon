//
// Created by gaeqs on 28/05/2025.
//

#include "VKTextureView.h"

#include <vulkan/AbstractVKApplication.h>
#include <vulkan/util/VKUtil.h>
#include <vulkan/util/VulkanConversions.h>

namespace neon::vulkan
{

    namespace vc = conversions;

    VKTextureView::VKTextureView(Application* application, std::string name, const TextureViewCreateInfo& info,
                                 std::shared_ptr<Texture> texture) :
        VKResource(application),
        TextureView(std::move(name), info, std::move(texture))
    {
        auto native = static_cast<VkImage>(getTexture()->getNativeHandle());
        VkFormat format;
        if (info.format) {
            format = vc::vkFormat(info.format.value());
        } else {
            format = vc::vkFormat(getTexture()->getFormat());
        }

        VkImageAspectFlags flags = 0;
        for (auto aspect : info.aspect) {
            flags |= static_cast<VkImageAspectFlags>(aspect);
        }

        _view = vulkan_util::createImageView(getApplication()->getDevice()->hold(), native, format, flags, info,
                                             getTexture()->getTextureViewType());
    }

    VKTextureView::~VKTextureView()
    {
        getApplication()->getBin()->destroyLater(getApplication()->getDevice(), getRuns(), _view, vkDestroyImageView);
    }

    void* VKTextureView::getNativeHandle()
    {
        return _view;
    }

    const void* VKTextureView::getNativeHandle() const
    {
        return _view;
    }

    VkImageView VKTextureView::vk() const
    {
        return _view;
    }

} // namespace neon::vulkan