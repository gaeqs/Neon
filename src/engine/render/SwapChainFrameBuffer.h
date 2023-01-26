//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_SWAPCHAINFRAMEBUFFER_H
#define NEON_SWAPCHAINFRAMEBUFFER_H

#include <engine/render/FrameBuffer.h>

#ifdef USE_VULKAN

#include "vulkan/render/VKSwapChainFrameBuffer.h"

#endif

class Room;

class SwapChainFrameBuffer : public FrameBuffer {

public:

#ifdef USE_VULKAN
    using Implementation = VKSwapChainFrameBuffer;
#endif

private:

    Implementation _implementation;

public:

    SwapChainFrameBuffer(Room* room, bool depth);

    ~SwapChainFrameBuffer() override = default;

    void recreate() override;

    [[nodiscard]] FrameBuffer::Implementation& getImplementation() override;

    [[nodiscard]] const FrameBuffer::Implementation&
    getImplementation() const override;

    [[nodiscard]] std::vector<IdentifiableWrapper<Texture>>
    getTextures() const override;

    [[nodiscard]] uint32_t getWidth() const override;

    [[nodiscard]] uint32_t getHeight() const override;
};


#endif //NEON_SWAPCHAINFRAMEBUFFER_H
