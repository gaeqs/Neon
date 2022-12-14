//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_VKFRAMEBUFFER_H
#define NEON_VKFRAMEBUFFER_H


#include <vulkan/vulkan.h>

class VKFrameBuffer {

public:

    VKFrameBuffer(const VKFrameBuffer& other) = delete;

    VKFrameBuffer() = default;

    virtual ~VKFrameBuffer() = 0;

    [[nodiscard]] virtual VkFramebuffer getRaw() const = 0;

};


#endif //NEON_VKFRAMEBUFFER_H
