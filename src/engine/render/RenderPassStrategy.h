//
// Created by gaelr on 22/12/2022.
//

#ifndef NEON_RENDERPASSSTRATEGY_H
#define NEON_RENDERPASSSTRATEGY_H

#include <functional>

#include <engine/render/RenderPass.h>

class Room;

struct RenderPassStrategy {

    std::shared_ptr<FrameBuffer> frameBuffer;
    RenderPass renderPass;
    std::function<void(Room*)> strategy;

    RenderPassStrategy(
            Application* _application,
            const std::shared_ptr<FrameBuffer>& _frameBuffer,
            const std::function<void(Room*)>& _strategy) :
            frameBuffer(_frameBuffer),
            renderPass(_application, *_frameBuffer),
            strategy(_strategy) {};
};

#endif //NEON_RENDERPASSSTRATEGY_H
