//
// Created by gaelr on 25/12/2022.
//

#include "RenderPassStrategy.h"

#include <engine/model/Model.h>
#include <engine/structure/Room.h>
#include <engine/render/FrameBuffer.h>

#include <vulkan/render/VKRenderPass.h>

namespace neon {
    RenderPassStrategy::RenderPassStrategy(
            const std::shared_ptr<FrameBuffer>& _frameBuffer,
            const std::function<void(
                    Room*,
                    std::shared_ptr<FrameBuffer>)>& _strategy) :
            frameBuffer(_frameBuffer),
            strategy(_strategy) {

    }

    void RenderPassStrategy::defaultStrategy(
            Room* room,
            const std::shared_ptr<FrameBuffer>& target) {
        auto& app = room->getApplication()->getImplementation();
        auto renderPass = target->getImplementation().getRenderPass().getRaw();
        auto globalUniformBuffer = &room->getApplication()->getRender()
                ->getGlobalUniformBuffer();
        for (const auto& [model, amount]: room->usedModels()) {
            model->getImplementation().draw(
                    app.getCurrentCommandBuffer(),
                    renderPass,
                    globalUniformBuffer
            );
        }
    }
}
