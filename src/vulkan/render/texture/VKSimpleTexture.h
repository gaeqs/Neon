//
// Created by gaeqs on 28/05/2025.
//

#ifndef NEON_VKSIMPLETEXTURE_H
#define NEON_VKSIMPLETEXTURE_H

#include <imgui.h>
#include <neon/render/texture/Texture.h>
#include <neon/render/texture/TextureCreateInfo.h>

namespace neon::vulkan
{
    class VKSimpleTexture :
        public VKResource,
        public Texture,
        public TextureCapabilityModifiable
    {
        VkImage _image;
        VmaAllocation _allocation;
        ImageCreateInfo _info;

        VkImageLayout _currentLayout;

        void transitionLayout(VkImageLayout layout, VkCommandBuffer commandBuffer);

        void uploadData(const std::byte* data, CommandBuffer* commandBuffer);

        void generateMipmaps(VkCommandBuffer buffer);

      public:
        VKSimpleTexture(Application* application, std::string name, const ImageCreateInfo& info,
                        const std::byte* data, CommandBuffer* commandBuffer = nullptr);

        ~VKSimpleTexture() override;

        [[nodiscard]] rush::Vec3ui getDimensions() const override;

        [[nodiscard]] TextureFormat getFormat() const override;

        [[nodiscard]] SamplesPerTexel getSamplesPerTexel() const override;

        [[nodiscard]] size_t getNumberOfMipmaps() const override;

        [[nodiscard]] std::optional<const TextureCapabilityRead*> asReadable() const override;

        [[nodiscard]] std::optional<TextureCapabilityModifiable*> asModifiable() override;

        [[nodiscard]] void* getNativeHandle() override;

        [[nodiscard]] const void* getNativeHandle() const override;

        Result<void, std::string> updateData(const std::byte* data, rush::Vec3ui offset, rush::Vec3ui size,
                                             uint32_t layerOffset, uint32_t layers,
                                             CommandBuffer* commandBuffer) override;
    };
} // namespace neon::vulkan

#endif // NEON_VKSIMPLETEXTURE_H
