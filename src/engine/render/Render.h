//
// Created by gaelr on 22/12/2022.
//

#ifndef NEON_RENDER_H
#define NEON_RENDER_H

#include <functional>
#include <vector>
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
    std::vector<RenderPassStrategy> _strategies;

public:

    Render(const Render& other) = delete;

    explicit Render(Application* application);

    [[nodiscard]] const Implementation& getImplementation() const;

    [[nodiscard]] Implementation& getImplementation();

    void addRenderPass(const RenderPassStrategy& strategy);

    void clearRenderPasses();

    void render(Room* room) const;

    void recreateFrameBuffers();

    [[nodiscard]] size_t getPassesAmount() const;

    [[nodiscard]] std::shared_ptr<FrameBuffer> getFrameBuffer (size_t index);
};


#endif //NEON_RENDER_H
