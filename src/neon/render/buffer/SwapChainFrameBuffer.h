//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_SWAPCHAINFRAMEBUFFER_H
#define NEON_SWAPCHAINFRAMEBUFFER_H

#include <neon/render/buffer/FrameBuffer.h>

#ifdef USE_VULKAN

    #include "vulkan/render/VKSwapChainFrameBuffer.h"

#endif

namespace neon
{
    class Application;

    class SwapChainFrameBuffer : public FrameBuffer
    {
      public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKSwapChainFrameBuffer;
#endif

      private:
        Implementation _implementation;

      public:
        SwapChainFrameBuffer(Application* application, std::string name, SamplesPerTexel samples, bool depth);

        ~SwapChainFrameBuffer() override = default;

        bool requiresRecreation() override;

        void recreate() override;

        [[nodiscard]] FrameBuffer::Implementation& getImplementation() override;

        [[nodiscard]] const FrameBuffer::Implementation& getImplementation() const override;

        [[nodiscard]] std::vector<FrameBufferOutput> getOutputs() const override;

        [[nodiscard]] uint32_t getWidth() const override;

        [[nodiscard]] uint32_t getHeight() const override;

        SamplesPerTexel getSamples() const override;
    };
} // namespace neon

#endif //NEON_SWAPCHAINFRAMEBUFFER_H
