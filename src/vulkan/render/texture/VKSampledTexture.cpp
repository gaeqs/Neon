//
// Created by gaeqs on 28/05/2025.
//

#include "VKSampledTexture.h"

#include <imgui_impl_vulkan.h>

namespace neon::vulkan
{

    VKSampledTexture::VKSampledTexture(std::shared_ptr<TextureView> view, std::shared_ptr<Sampler> sampler) :
        SampledTexture(std::move(view), std::move(sampler))
    {
        auto vkSampler = static_cast<VkSampler>(getSampler()->getNativeHandle());
        auto vkView = static_cast<VkImageView>(getView()->getNativeHandle());
        _set = ImGui_ImplVulkan_AddTexture(vkSampler, vkView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    ImTextureID VKSampledTexture::getImGuiDescriptor() const
    {
        return reinterpret_cast<ImTextureID>(_set);
    }
} // namespace neon::vulkan