//
// Created by grial on 15/12/22.
//

#ifndef NEON_VKSIMPLEFRAMEBUFFER_H
#define NEON_VKSIMPLEFRAMEBUFFER_H

#include <vector>

#include <vulkan/render/VKFrameBuffer.h>

class Application;

class VKApplication;

class VKSimpleFrameBuffer : public VKFrameBuffer {

    VKApplication* _vkApplication;

    VkFramebuffer _frameBuffer;

    std::vector<VkImage> _images;
    std::vector<VkDeviceMemory> _memories;
    std::vector<VkImageView> _imageViews;

    std::vector<VkFormat> _formats;
    bool _depth;

    void createImages();

    void createFrameBuffer();

    void cleanup();


public:

    VKSimpleFrameBuffer(Application* application,
                        std::vector<VkFormat> formats, bool depth);

    ~VKSimpleFrameBuffer() override;

    [[nodiscard]] VkFramebuffer getRaw() const override;

    [[nodiscard]] bool hasDepth() override;

    [[nodiscard]] uint32_t getColorAttachmentAmount() override;

    void recreate();

};


#endif //NEON_VKSIMPLEFRAMEBUFFER_H
