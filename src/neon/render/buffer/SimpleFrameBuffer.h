//
// Created by grial on 15/12/22.
//

#ifndef NEON_SIMPLEFRAMEBUFFER_H
#define NEON_SIMPLEFRAMEBUFFER_H

#include <functional>

#include <neon/render/buffer/FrameBuffer.h>
#include <neon/render/buffer/FrameBufferTextureCreateInfo.h>

#ifdef USE_VULKAN

    #include <vulkan/render/buffer/VKSimpleFrameBuffer.h>

#endif

namespace neon
{
    class Application;

    class SimpleFrameBuffer : public FrameBuffer
    {
      public:
        using Condition = std::function<bool(const SimpleFrameBuffer*)>;

        using Parameters = std::function<rush::Vec2ui(Application*)>;

#ifdef USE_VULKAN
        using Implementation = vulkan::VKSimpleFrameBuffer;
#endif

      private:
        Application* _application;
        Implementation _implementation;

        Condition _recreationCondition;
        Parameters _recreationParameters;

      public:
        static bool defaultRecreationCondition(const SimpleFrameBuffer*);

        static rush::Vec2ui defaultRecreationParameters(Application*);

        SimpleFrameBuffer(Application* application, std::string name, SamplesPerTexel samples,
                          const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                          const std::optional<FrameBufferDepthCreateInfo>& depthInfo = {},
                          Condition condition = defaultRecreationCondition,
                          Parameters parameters = defaultRecreationParameters);

        ~SimpleFrameBuffer() override = default;

        bool requiresRecreation() override;

        void recreate() override;

        [[nodiscard]] FrameBuffer::Implementation& getImplementation() override;

        [[nodiscard]] const FrameBuffer::Implementation& getImplementation() const override;

        std::vector<FrameBufferOutput> getOutputs() const override;

        rush::Vec2ui getDimensions() const override;

        SamplesPerTexel getSamples() const override;

        [[nodiscard]] const Condition& getRecreationCondition() const;

        void setRecreationCondition(const Condition& recreationCondition);

        [[nodiscard]] const Parameters& getRecreationParameters() const;

        void setRecreationParameters(const Parameters& recreationParameters);
    };
} // namespace neon

#endif // NEON_SIMPLEFRAMEBUFFER_H
