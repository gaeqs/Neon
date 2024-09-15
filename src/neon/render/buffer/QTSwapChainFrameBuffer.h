//
// Created by gaelr on 14/12/2022.
//


#ifndef NEON_QTSWAPCHAINFRAMEBUFFER_H
#define NEON_QTSWAPCHAINFRAMEBUFFER_H

#ifdef USE_QT

#include <neon/render/buffer/FrameBuffer.h>

#ifdef USE_VULKAN

#include "vulkan/render/VKQTSwapChainFrameBuffer.h"

#endif

namespace neon {

    class Application;

    class QTSwapChainFrameBuffer : public FrameBuffer {

    public:

#ifdef USE_VULKAN
        using Implementation = vulkan::VKQTSwapChainFrameBuffer;
#endif

    private:

        Implementation _implementation;

    public:

        explicit QTSwapChainFrameBuffer(Application* application);

        ~QTSwapChainFrameBuffer() override = default;

        bool requiresRecreation() override;

        void recreate() override;

        [[nodiscard]] FrameBuffer::Implementation& getImplementation() override;

        [[nodiscard]] const FrameBuffer::Implementation&
        getImplementation() const override;

        [[nodiscard]] std::vector<std::shared_ptr<Texture>>
        getTextures() const override;

        [[nodiscard]] uint32_t getWidth() const override;

        [[nodiscard]] uint32_t getHeight() const override;
    };
}

#endif

#endif //NEON_QTSWAPCHAINFRAMEBUFFER_H
