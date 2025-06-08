//
// Created by gaeqs on 28/05/2025.
//

#ifndef NEON_VKSAMPLEDTEXTURE_H
#define NEON_VKSAMPLEDTEXTURE_H

#include <neon/render/texture/SampledTexture.h>

namespace neon::vulkan
{
    class VKSampledTexture : public SampledTexture
    {
        mutable VkDescriptorSet _set;
        mutable uint64_t _setVersion;

      public:
        VKSampledTexture(std::string name, std::shared_ptr<MutableAsset<TextureView>> view,
                         std::shared_ptr<Sampler> sampler);

        ~VKSampledTexture() override;

        [[nodiscard]] ImTextureID getImGuiDescriptor() const override;
    };
} // namespace neon::vulkan

#endif // VKIMGUITEXTUREMANAGER_H
