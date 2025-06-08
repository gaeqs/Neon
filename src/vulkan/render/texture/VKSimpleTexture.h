//
// Created by gaeqs on 28/05/2025.
//

#ifndef NEON_VKSIMPLETEXTURE_H
#define NEON_VKSIMPLETEXTURE_H

#include <neon/render/texture/Texture.h>
#include <neon/render/texture/TextureCreateInfo.h>
#include <vulkan/VKResource.h>

namespace neon::vulkan
{
    class VKSimpleTexture :
        public VKResource,
        public Texture,
        public TextureCapabilityRead,
        public TextureCapabilityModifiable
    {
        VkImage _image;
        VmaAllocation _allocation;
        TextureCreateInfo _info;

        mutable VkImageLayout _currentLayout;

        void transitionLayout(VkImageLayout layout, VkCommandBuffer commandBuffer) const;

        void uploadData(const std::byte* data, CommandBuffer* commandBuffer);

        void generateMipmaps(VkCommandBuffer buffer);

      public:
        VKSimpleTexture(Application* application, std::string name, const TextureCreateInfo& info,
                        const std::byte* data, CommandBuffer* commandBuffer = nullptr);

        ~VKSimpleTexture() override;

        [[nodiscard]] rush::Vec3ui getDimensions() const override;

        [[nodiscard]] TextureFormat getFormat() const override;

        [[nodiscard]] SamplesPerTexel getSamplesPerTexel() const override;

        [[nodiscard]] TextureViewType getTextureViewType() const override;

        [[nodiscard]] size_t getNumberOfMipmaps() const override;

        [[nodiscard]] std::optional<const TextureCapabilityRead*> asReadable() const override;

        [[nodiscard]] std::optional<TextureCapabilityModifiable*> asModifiable() override;

        [[nodiscard]] size_t getAllocatedMemoryInBytes() const override;

        [[nodiscard]] void* getNativeHandle() override;

        [[nodiscard]] const void* getNativeHandle() const override;

        [[nodiscard]] std::any getLayoutNativeHandle() const override;

        Result<void, std::string> readData(std::byte* data, rush::Vec3ui offset, rush::Vec3ui size,
                                           uint32_t layerOffset, uint32_t layers) const override;

        Result<void, std::string> updateData(const std::byte* data, rush::Vec3ui offset, rush::Vec3ui size,
                                             uint32_t layerOffset, uint32_t layers,
                                             CommandBuffer* commandBuffer) override;

        [[nodiscard]] VkImage vk() const;

        [[nodiscard]] VkImageLayout vkLayout() const;
    };
} // namespace neon::vulkan

#endif // NEON_VKSIMPLETEXTURE_H
