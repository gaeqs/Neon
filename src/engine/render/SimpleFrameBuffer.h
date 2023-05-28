//
// Created by grial on 15/12/22.
//

#ifndef NEON_SIMPLEFRAMEBUFFER_H
#define NEON_SIMPLEFRAMEBUFFER_H

#include <functional>

#include <engine/render/FrameBuffer.h>
#include <engine/render/TextureCreateInfo.h>
#include <engine/render/FrameBufferTextureCreateInfo.h>

#ifdef USE_VULKAN

#include <vulkan/render/VKSimpleFrameBuffer.h>

#endif

namespace neon {

    class Application;

    class SimpleFrameBuffer : public FrameBuffer {

    public:

        using Condition = std::function<bool(const SimpleFrameBuffer*)>;

        using Parameters = std::function<std::pair<uint32_t, uint32_t>(
                Application*)>;

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

        static std::pair<uint32_t, uint32_t>
        defaultRecreationParameters(Application*);

        SimpleFrameBuffer(Application* application,
                          const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                          bool depth,
                          Condition condition = defaultRecreationCondition,
                          const Parameters& parameters = defaultRecreationParameters);

        SimpleFrameBuffer(Application* application,
                          const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                          std::shared_ptr<Texture> depthTexture,
                          Condition condition = defaultRecreationCondition,
                          const Parameters& parameters = defaultRecreationParameters);

        ~SimpleFrameBuffer() override = default;

        bool requiresRecreation() override;

        void recreate() override;

        [[nodiscard]] FrameBuffer::Implementation& getImplementation() override;

        [[nodiscard]] const FrameBuffer::Implementation&
        getImplementation() const override;

        [[nodiscard]] std::vector<std::shared_ptr<Texture>>
        getTextures() const override;

        [[nodiscard]] uint32_t getWidth() const override;

        [[nodiscard]] uint32_t getHeight() const override;

        [[nodiscard]] const Condition& getRecreationCondition() const;

        void setRecreationCondition(const Condition& recreationCondition);

        [[nodiscard]] const Parameters& getRecreationParameters() const;

        void setRecreationParameters(const Parameters& recreationParameters);

        [[nodiscard]] ImTextureID getImGuiDescriptor(uint32_t index);
    };
}


#endif //NEON_SIMPLEFRAMEBUFFER_H
