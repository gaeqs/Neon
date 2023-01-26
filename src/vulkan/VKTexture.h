//
// Created by gaelr on 10/11/2022.
//

#ifndef NEON_VKTEXTURE_H
#define NEON_VKTEXTURE_H


#include <cstdint>
#include <engine/render/TextureFormat.h>
#include <vulkan/buffer/SimpleBuffer.h>

class Application;

class VKApplication;

class VKTexture {

    VKApplication* _vkApplication;

    int32_t _width, _height;
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

    static uint32_t getPixelSize(TextureFormat format);

public:


    VKTexture(const VKTexture& other) = delete;

    VKTexture(Application* application, const void* data,
              int32_t width, int32_t height,
              TextureFormat format);

    VKTexture(Application* application,
              int32_t width, int32_t height,
              VkImageView imageView,
              VkImageLayout layout);

    ~VKTexture();

    [[nodiscard]] int32_t getWidth() const;

    [[nodiscard]] int32_t getHeight() const;

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

    void changeExternalImageView(
            int32_t width, int32_t height, VkImageView imageView);

    void updateData(const void* data, int32_t width, int32_t height,
                    TextureFormat format);

};


#endif //NEON_VKTEXTURE_H
