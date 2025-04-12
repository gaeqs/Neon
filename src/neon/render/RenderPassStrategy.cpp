//
// Created by gaelr on 25/12/2022.
//

#include "RenderPassStrategy.h"

#include <neon/render/model/Model.h>
#include <neon/structure/Room.h>
#include <neon/render/buffer/FrameBuffer.h>
#include <neon/render/Render.h>
#include <neon/render/shader/Material.h>

namespace neon
{
    RenderPassStrategy::RenderPassStrategy(std::string name, int priority) :
        Asset(typeid(RenderPassStrategy), std::move(name)),
        _priority(priority)
    {
    }

    int RenderPassStrategy::getPriority() const
    {
        return _priority;
    }

    DefaultRenderPassStrategy::DefaultRenderPassStrategy(std::string name,
                                                         const std::shared_ptr<FrameBuffer>& frameBuffer,
                                                         int priority) :
        RenderPassStrategy(std::move(name), priority),
        _frameBuffer(frameBuffer)
    {
    }

    const std::shared_ptr<FrameBuffer>& DefaultRenderPassStrategy::getFrameBuffer() const
    {
        return _frameBuffer;
    }

    void DefaultRenderPassStrategy::render(Room* room, const Render* render,
                                           const std::vector<std::shared_ptr<Material>>& materials) const
    {
        render->beginRenderPass(_frameBuffer);
        if (room != nullptr) {
            for (const auto& material : materials) {
                if (material->getTarget() != _frameBuffer) {
                    continue;
                }
                for (const auto& [model, amount] : room->usedModels()) {
                    model->draw(material.get());
                }
            }
        }
        render->endRenderPass();
    }

    bool DefaultRenderPassStrategy::requiresRecreation()
    {
        return _frameBuffer->requiresRecreation();
    }

    void DefaultRenderPassStrategy::recreate()
    {
        _frameBuffer->recreate();
    }
} // namespace neon
