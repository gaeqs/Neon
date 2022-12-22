//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_FRAMEBUFFER_H
#define NEON_FRAMEBUFFER_H

#ifdef USE_VULKAN

#include "vulkan/render/VKFrameBuffer.h"

#endif

class FrameBuffer {

public:

#ifdef USE_VULKAN
    using Implementation = VKFrameBuffer;
#endif

    FrameBuffer() = default;

    FrameBuffer(const FrameBuffer& other) = delete;

    virtual ~FrameBuffer() = default;

    virtual void recreate() = 0;

    [[nodiscard]] virtual Implementation& getImplementation() = 0;

    [[nodiscard]] virtual const Implementation& getImplementation() const = 0;

};


#endif //NEON_FRAMEBUFFER_H
