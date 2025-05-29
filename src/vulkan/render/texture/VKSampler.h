//
// Created by gaeqs on 28/05/2025.
//

#ifndef NEON_VKSAMPLER_H
#define NEON_VKSAMPLER_H

#include <neon/render/texture/Sampler.h>
#include <vulkan/VKResource.h>

namespace neon::vulkan
{
    class VKSampler : public VKResource, public Sampler
    {
        VkSampler _sampler;

      public:
        VKSampler(Application* application, std::string name, const SamplerCreateInfo& info);

        ~VKSampler() override;

        [[nodiscard]] void* getNativeHandle() override;

        [[nodiscard]] const void* getNativeHandle() const override;
    };
} // namespace neon::vulkan

#endif // VKSAMPLER_H
