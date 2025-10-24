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

namespace neon
{

    class Application;

    class QTSwapChainFrameBuffer : public FrameBuffer
    {
      public:
    #ifdef USE_VULKAN
        using Implementation = vulkan::VKQTSwapChainFrameBuffer;
    #endif

      private:
        Implementation _implementation;

      public:
        QTSwapChainFrameBuffer(Application* application, std::string name);

        ~QTSwapChainFrameBuffer() override = default;

        bool requiresRecreation() override;

        void recreate() override;

        [[nodiscard]] FrameBuffer::Implementation& getImplementation() override;

        [[nodiscard]] const FrameBuffer::Implementation& getImplementation() const override;

        [[nodiscard]] rush::Vec2ui getDimensions() const override;

        [[nodiscard]] std::vector<FrameBufferOutput> getOutputs() const override;

        [[nodiscard]] SamplesPerTexel getSamples() const override;
    };
} // namespace neon

#endif

#endif // NEON_QTSWAPCHAINFRAMEBUFFER_H
