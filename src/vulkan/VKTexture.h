//
// Created by gaelr on 10/11/2022.
//

#ifndef NEON_VKTEXTURE_H
#define NEON_VKTEXTURE_H


#include <cstdint>
#include <engine/TextureFormat.h>
#include <vulkan/buffer/SimpleBuffer.h>

class Application;

class VKApplication;

class VKTexture {

    VKApplication* _vkApplication;

    int32_t _width, _height;
    SimpleBuffer _stagingBuffer;

    VkImage _image;
    VkDeviceMemory _imageMemory;
    VkImageView _imageView;
    VkSampler _sampler;

    static uint32_t getPixelSize(TextureFormat format);

    static VkFormat getImageFormat(TextureFormat format);


public:


    VKTexture(const VKTexture& other) = delete;

    VKTexture(Application* application, const void* data,
              int32_t width, int32_t height,
              TextureFormat format);

    ~VKTexture();

    [[nodiscard]] int32_t getWidth() const;

    [[nodiscard]] int32_t getHeight() const;

    [[nodiscard]] VkImageView getImageView() const;

    [[nodiscard]] VkSampler getSampler() const;

    void updateData(const void* data, int32_t width, int32_t height,
                    TextureFormat format);

};


#endif //NEON_VKTEXTURE_H
