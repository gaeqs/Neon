//
// Created by gaelr on 22/12/2022.
//

#ifndef NEON_RENDERPASSSTRATEGY_H
#define NEON_RENDERPASSSTRATEGY_H

#include <functional>
#include <memory>

class Room;

class FrameBuffer;

struct RenderPassStrategy {

    std::shared_ptr<FrameBuffer> frameBuffer;
    std::function<void(Room*, std::shared_ptr<FrameBuffer>)> strategy;

    RenderPassStrategy(
            const std::shared_ptr<FrameBuffer>& _frameBuffer,
            const std::function<void(Room*, std::shared_ptr<FrameBuffer>)>&
            _strategy);

    static void defaultStrategy(
            Room* room,
            const std::shared_ptr<FrameBuffer>& target);
};

#endif //NEON_RENDERPASSSTRATEGY_H
