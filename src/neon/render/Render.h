//
// Created by gaelr on 22/12/2022.
//

#ifndef NEON_RENDER_H
#define NEON_RENDER_H

#include <set>

#include <neon/render/RenderPassStrategy.h>
#include <neon/structure/Asset.h>

#include <neon/render/shader/ShaderUniformBuffer.h>
#include <neon/render/shader/ShaderUniformDescriptor.h>

#ifdef USE_VULKAN

    #include <vulkan/render/VKRender.h>

#endif

namespace neon
{

    class Room;

    class Application;

    struct RenderEntry
    {
        std::shared_ptr<RenderPassStrategy> strategy;
        size_t subId;
    };

    struct PrioritizedRenderPassStrategyComparer
    {
        bool operator()(const RenderEntry& a, const RenderEntry& b) const
        {
            if (a.strategy->getPriority() == b.strategy->getPriority()) {
                return a.subId < b.subId;
            }
            return a.strategy->getPriority() < b.strategy->getPriority();
        }
    };

    class Render : public Asset
    {
      public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKRender;
#endif

      private:
        Implementation _implementation;
        Application* _application;
        std::set<RenderEntry, PrioritizedRenderPassStrategyComparer> _strategies;

        std::shared_ptr<ShaderUniformDescriptor> _globalUniformDescriptor;
        ShaderUniformBuffer _globalUniformBuffer;

      public:
        Render(const Render& other) = delete;

        Render(Application* application, std::string name, const std::shared_ptr<ShaderUniformDescriptor>& descriptor);

        [[nodiscard]] const Implementation& getImplementation() const;

        [[nodiscard]] Implementation& getImplementation();

        void addRenderPass(const std::shared_ptr<RenderPassStrategy>& strategy);

        bool removeRenderPass(const std::shared_ptr<RenderPassStrategy>& strategy);

        void clearRenderPasses();

        void render(Room* room) const;

        void checkFrameBufferRecreationConditions();

        [[nodiscard]] size_t getStrategyAmount() const;

        [[nodiscard]] const std::shared_ptr<ShaderUniformDescriptor>& getGlobalUniformDescriptor() const;

        [[nodiscard]] const ShaderUniformBuffer& getGlobalUniformBuffer() const;

        [[nodiscard]] ShaderUniformBuffer& getGlobalUniformBuffer();

        auto getStrategies()
        {
            return _strategies | std::views::transform([](const RenderEntry& entry) { return entry.strategy; });
        }

        // region Strategy methods

        void beginRenderPass(const std::shared_ptr<FrameBuffer>& fb, bool clear = true) const;

        void endRenderPass() const;

        // endregion
    };
} // namespace neon

#endif // NEON_RENDER_H
