//
// Created by gaelr on 22/12/2022.
//

#ifndef NEON_RENDER_H
#define NEON_RENDER_H

#include <functional>
#include <vector>

#include <engine/render/RenderPassStrategy.h>
#include <engine/structure/Asset.h>
#include <util/ClusteredLinkedCollection.h>

#include <engine/shader/ShaderUniformBuffer.h>
#include <engine/shader/ShaderUniformDescriptor.h>

#ifdef USE_VULKAN

#include <vulkan/render/VKRender.h>

#endif

namespace neon {

    class Room;

    class Application;

    class Render : public Asset {

    public:

#ifdef USE_VULKAN
        using Implementation = vulkan::VKRender;
#endif

    private:

        Implementation _implementation;
        Application* _application;
        std::vector<std::shared_ptr<RenderPassStrategy>> _strategies;

        std::shared_ptr<ShaderUniformDescriptor> _globalUniformDescriptor;
        ShaderUniformBuffer _globalUniformBuffer;

    public:

        Render(const Render& other) = delete;

        Render(Application* application,
               std::string name,
               const std::shared_ptr<ShaderUniformDescriptor>& descriptor);

        [[nodiscard]] const Implementation& getImplementation() const;

        [[nodiscard]] Implementation& getImplementation();

        void addRenderPass(const std::shared_ptr<RenderPassStrategy>& strategy);

        void clearRenderPasses();

        void render(Room* room) const;

        void checkFrameBufferRecreationConditions();

        [[nodiscard]] size_t getStrategyAmount() const;

        [[nodiscard]] const std::vector<std::shared_ptr<RenderPassStrategy>>&
        getStrategies() const;

        [[nodiscard]] const std::shared_ptr<ShaderUniformDescriptor>&
        getGlobalUniformDescriptor() const;

        [[nodiscard]] const ShaderUniformBuffer& getGlobalUniformBuffer() const;

        [[nodiscard]] ShaderUniformBuffer& getGlobalUniformBuffer();

        // region Strategy methods

        void beginRenderPass(const std::shared_ptr<FrameBuffer>& fb) const;

        void endRenderPass() const;

        // endregion
    };
}

#endif //NEON_RENDER_H
