//
// Created by gaelr on 14/12/2022.
//

#ifndef NEON_FRAMEBUFFER_H
#define NEON_FRAMEBUFFER_H

#include <vector>
#include <memory>
#include <engine/structure/IdentifiableWrapper.h>

#ifdef USE_VULKAN

#include <optional>
#include <unordered_map>

#include <vulkan/render/VKFrameBuffer.h>

#include <glm/glm.hpp>

#endif

namespace neon {

    class Texture;

    class FrameBuffer {

        std::unordered_map<uint32_t, glm::vec4> _clearColors;
        std::pair<float, uint32_t> _depthClear;

    public:

#ifdef USE_VULKAN
        using Implementation = vulkan::VKFrameBuffer;
#endif

        FrameBuffer();

        FrameBuffer(const FrameBuffer& other) = delete;

        virtual ~FrameBuffer() = default;

        [[nodiscard]] std::optional<glm::vec4>
        getClearColor(uint32_t index) const;

        void setClearColor(uint32_t index, glm::vec4 color);

        [[nodiscard]] std::pair<float, uint32_t> getDepthClearColor() const;

        void setDepthClearColor(float depth, uint32_t stencil);

        virtual bool requiresRecreation() = 0;

        virtual void recreate() = 0;

        [[nodiscard]] virtual Implementation& getImplementation() = 0;

        [[nodiscard]] virtual const Implementation&
        getImplementation() const = 0;

        [[nodiscard]] virtual std::vector<std::shared_ptr<Texture>>
        getTextures() const = 0;

        [[nodiscard]] virtual uint32_t getWidth() const = 0;

        [[nodiscard]] virtual uint32_t getHeight() const = 0;
    };
}

#endif //NEON_FRAMEBUFFER_H
