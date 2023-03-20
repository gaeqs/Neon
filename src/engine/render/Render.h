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
        std::vector<RenderPassStrategy> _strategies;

    public:

        Render(const Render& other) = delete;

        explicit Render(Application* application, std::string name);

        [[nodiscard]] const Implementation& getImplementation() const;

        [[nodiscard]] Implementation& getImplementation();

        void addRenderPass(const RenderPassStrategy& strategy);

        void clearRenderPasses();

        void render(Room* room) const;

        void checkFrameBufferRecreationConditions();

        [[nodiscard]] size_t getPassesAmount() const;

        [[nodiscard]] std::shared_ptr<FrameBuffer> getFrameBuffer(size_t index);
    };
}

#endif //NEON_RENDER_H
