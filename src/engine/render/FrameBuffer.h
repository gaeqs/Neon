//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_FRAMEBUFFER_H
#define NEON_FRAMEBUFFER_H

#include <vector>
#include <engine/structure/IdentifiableWrapper.h>

#ifdef USE_VULKAN

#include <vulkan/render/VKFrameBuffer.h>

#endif

class Texture;

class FrameBuffer {

public:

#ifdef USE_VULKAN
    using Implementation = VKFrameBuffer;
#endif

    FrameBuffer() = default;

    FrameBuffer(const FrameBuffer& other) = delete;

    virtual ~FrameBuffer() = default;

    virtual bool requiresRecreation() = 0;

    virtual void recreate() = 0;

    [[nodiscard]] virtual Implementation& getImplementation() = 0;

    [[nodiscard]] virtual const Implementation& getImplementation() const = 0;

    [[nodiscard]] virtual std::vector<IdentifiableWrapper<Texture>>
    getTextures() const = 0;

    [[nodiscard]] virtual uint32_t getWidth() const = 0;

    [[nodiscard]] virtual uint32_t getHeight() const = 0;
};


#endif //NEON_FRAMEBUFFER_H