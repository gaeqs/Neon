//
// Created by gaeqs on 28/05/2025.
//

#include "VKSampledTexture.h"

#include <imgui_impl_vulkan.h>

namespace neon::vulkan
{

    VKSampledTexture::VKSampledTexture(std::string name, std::shared_ptr<MutableAsset<TextureView>> view,
                                       std::shared_ptr<Sampler> sampler) :
        SampledTexture(std::move(name), std::move(view), std::move(sampler)),
        _set(nullptr),
        _setVersion(0)
    {
    }

    VKSampledTexture::~VKSampledTexture()
    {
        if (_set != nullptr) {
            ImGui_ImplVulkan_RemoveTexture(_set);
        }
    }

    ImTextureID VKSampledTexture::getImGuiDescriptor() const
    {
        if (_set == nullptr || _setVersion != getViewVersion()) {
            if (_set != nullptr) {
                ImGui_ImplVulkan_RemoveTexture(_set);
            }
            auto vkSampler = static_cast<VkSampler>(getSampler()->getNativeHandle());
            auto vkView = static_cast<VkImageView>(getView()->get()->getNativeHandle());
            _set = ImGui_ImplVulkan_AddTexture(vkSampler, vkView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            _setVersion = getViewVersion();
        }
        return reinterpret_cast<ImTextureID>(_set);
    }
} // namespace neon::vulkan