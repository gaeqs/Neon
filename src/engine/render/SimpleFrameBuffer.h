//
// Created by grial on 15/12/22.
//

#ifndef NEON_SIMPLEFRAMEBUFFER_H
#define NEON_SIMPLEFRAMEBUFFER_H

#include <functional>

#include <engine/render/FrameBuffer.h>
#include <engine/render/TextureCreateInfo.h>

#ifdef USE_VULKAN

#include <vulkan/render/VKSimpleFrameBuffer.h>

#endif

namespace neon {

    class Application;

    class SimpleFrameBuffer : public FrameBuffer {

    public:

#ifdef USE_VULKAN
    using Implementation = vulkan::VKSimpleFrameBuffer;
#endif

    private:

        Implementation _implementation;

        std::function<bool(const SimpleFrameBuffer*)> _recreationCondition;
        std::function<std::pair<uint32_t, uint32_t>(const SimpleFrameBuffer*)>
                _recreationParameters;

        static bool defaultRecreationCondition(const SimpleFrameBuffer*);

        static std::pair<uint32_t, uint32_t>
        defaultRecreationParameters(const SimpleFrameBuffer*);

    public:

        SimpleFrameBuffer(Application* application,
                          const std::vector<TextureFormat>& colorFormats,
                          bool depth);

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

        [[nodiscard]] const std::function<bool(const SimpleFrameBuffer*)>&
        getRecreationCondition() const;

        void setRecreationCondition(const std::function<bool(
                const SimpleFrameBuffer*)>& recreationCondition);

        [[nodiscard]] const std::function<std::pair<uint32_t, uint32_t>(
                const SimpleFrameBuffer*)>& getRecreationParameters() const;

        void setRecreationParameters(
                const std::function<std::pair<uint32_t, uint32_t>(
                        const SimpleFrameBuffer*)>& recreationParameters);

        [[nodiscard]] ImTextureID getImGuiDescriptor(uint32_t index);
    };
}


#endif //NEON_SIMPLEFRAMEBUFFER_H
