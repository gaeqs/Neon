//
// Created by gaelr on 22/12/2022.
//

#ifndef NEON_RENDER_H
#define NEON_RENDER_H

#include <functional>
#include <engine/render/RenderPass.h>
#include <engine/render/RenderPassStrategy.h>
#include <util/ClusteredLinkedCollection.h>

#ifdef USE_VULKAN

#include <vulkan/render/VKRender.h>

#endif

class Room;

class Application;

class Render {

public:

#ifdef USE_VULKAN
    using Implementation = VKRender;
#endif

private:

    Implementation _implementation;
    Application* _application;
    ClusteredLinkedCollection<RenderPassStrategy> _strategies;

public:

    Render(const Render& other) = delete;

    explicit Render(Application* application);

    [[nodiscard]] const Implementation& getImplementation() const;

    [[nodiscard]] Implementation& getImplementation();

    void addRenderPass(const std::shared_ptr<FrameBuffer>& _frameBuffer,
                       const std::function<void(Room*)>& strategy);

    void clearRenderPasses();

    void render(Room* room) const;
};


#endif //NEON_RENDER_H
