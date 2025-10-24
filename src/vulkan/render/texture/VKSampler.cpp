//
// Created by gaeqs on 28/05/2025.
//

#include "VKSampler.h"

#include <vulkan/AbstractVKApplication.h>
#include <vulkan/util/VulkanConversions.h>

namespace vc = neon::vulkan::conversions;

namespace neon::vulkan
{
    VKSampler::VKSampler(Application* application, std::string name, const SamplerCreateInfo& info) :
        VKResource(application),
        Sampler(std::move(name), info)
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(getApplication()->getPhysicalDevice().getRaw(), &properties);
        auto samplerInfo = vc::vkSamplerCreateInfo(info, VK_LOD_CLAMP_NONE, properties.limits.maxSamplerAnisotropy);

        if (vkCreateSampler(getApplication()->getDevice()->hold(), &samplerInfo, nullptr, &_sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture sampler!");
        }
    }

    VKSampler::~VKSampler()
    {
        getApplication()->getBin()->destroyLater(getApplication()->getDevice(), getRuns(), _sampler, vkDestroySampler);
    }

    void* VKSampler::getNativeHandle()
    {
        return _sampler;
    }

    const void* VKSampler::getNativeHandle() const
    {
        return _sampler;
    }
} // namespace neon::vulkan
