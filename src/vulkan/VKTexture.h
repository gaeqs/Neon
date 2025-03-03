//
// Created by gaelr on 10/11/2022.
//

#ifndef NEON_VKTEXTURE_H
#define NEON_VKTEXTURE_H


#include <cstdint>
#include <vulkan/render/buffer/SimpleBuffer.h>
#include <neon/render/texture/TextureCreateInfo.h>
#include <rush/vector/vec.h>

namespace neon {
    class Application;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKTexture {
        Application* _application;
        AbstractVKApplication* _vkApplication;

        TextureFormat _format;
        int32_t _width, _height, _depth;
        uint32_t _mipmapLevels, _layers;
        std::unique_ptr<SimpleBuffer> _stagingBuffer;

        VkImage _image;
        VkDeviceMemory _imageMemory;
        VkImageView _imageView;
        VkSampler _sampler;
        VkImageLayout _layout;

        /**
         * If true, the VkImageView is managed by an external
         * system. StagingBuffer, image and imageMemory are null.
         */
        bool _external;
        uint32_t _externalDirtyFlag;

    public:
        VKTexture(const VKTexture& other) = delete;

        VKTexture(Application* application, const void* data,
                  const TextureCreateInfo& createInfo = TextureCreateInfo());

        VKTexture(Application* application,
                  TextureFormat format,
                  VkImage image,
                  VkDeviceMemory memory,
                  VkImageView imageView,
                  VkImageLayout layout,
                  uint32_t width, uint32_t height, uint32_t depth,
                  const SamplerCreateInfo& sampler = SamplerCreateInfo());

        ~VKTexture();

        [[nodiscard]] int32_t getWidth() const;

        [[nodiscard]] int32_t getHeight() const;

        [[nodiscard]] int32_t getDepth() const;

        [[nodiscard]] VkImage getImage() const;

        [[nodiscard]] VkDeviceMemory getMemory() const;

        [[nodiscard]] VkImageView getImageView() const;

        [[nodiscard]] VkSampler getSampler() const;

        [[nodiscard]] VkImageLayout getLayout() const;

        /**
         * A counter that it's incremented every time the external
         * image view is changed.
         * This counter starts at 1.
         * If the image view is internal, then this counter is always 1.
         * This counter can never be 0.
         *
         * @return the counter.
         */
        [[nodiscard]] uint32_t getExternalDirtyFlag() const;

        void makeInternal();

        void changeExternalImageView(
            int32_t width, int32_t height,
            VkImage image, VkDeviceMemory memory,
            VkImageView imageView);

        void updateData(const void* data,
                        int32_t width,
                        int32_t height,
                        int32_t depth,
                        TextureFormat format);

        void fetchData(void* data, rush::Vec3i offset, rush::Vec<3, uint32_t> size,
                       uint32_t layersOffset, uint32_t layers);
    };
}

#endif //NEON_VKTEXTURE_H
