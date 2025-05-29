//
// Created by gaeqs on 28/05/2025.
//

#ifndef NEON_VKSAMPLEDTEXTURE_H
#define NEON_VKSAMPLEDTEXTURE_H

#include <neon/render/texture/SampledTexture.h>
#include <vulkan/VKResource.h>

namespace neon::vulkan
{
    class VKSampledTexture : public SampledTexture
    {
        VkDescriptorSet _set;

      public:
        VKSampledTexture(std::shared_ptr<TextureView> view, std::shared_ptr<Sampler> sampler);

        ImTextureID getImGuiDescriptor() const override;
    };
} // namespace neon::vulkan

#endif // VKIMGUITEXTUREMANAGER_H
