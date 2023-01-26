//
// Created by grial on 15/12/22.
//

#ifndef NEON_SIMPLEFRAMEBUFFER_H
#define NEON_SIMPLEFRAMEBUFFER_H

#include <engine/render/FrameBuffer.h>
#include "TextureFormat.h"

#ifdef USE_VULKAN

#include <vulkan/render/VKSimpleFrameBuffer.h>

#endif

class Room;

class SimpleFrameBuffer : public FrameBuffer {

public:

#ifdef USE_VULKAN
    using Implementation = VKSimpleFrameBuffer;
#endif

private:

    Implementation _implementation;

public:

    SimpleFrameBuffer(Room* room,
                      const std::vector<TextureFormat>& colorFormats,
                      bool depth);

    ~SimpleFrameBuffer() override = default;

    void recreate() override;

    [[nodiscard]] FrameBuffer::Implementation& getImplementation() override;

    [[nodiscard]] const FrameBuffer::Implementation&
    getImplementation() const override;

    [[nodiscard]] std::vector<IdentifiableWrapper<Texture>>
    getTextures() const override;
};


#endif //NEON_SIMPLEFRAMEBUFFER_H
