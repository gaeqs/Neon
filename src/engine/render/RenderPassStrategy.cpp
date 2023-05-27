//
// Created by gaelr on 25/12/2022.
//

#include "RenderPassStrategy.h"

#include <engine/model/Model.h>
#include <engine/structure/Room.h>
#include <engine/render/FrameBuffer.h>
#include <engine/render/Render.h>
#include <engine/shader/Material.h>

namespace neon {

    DefaultRenderPassStrategy::DefaultRenderPassStrategy(
            const std::shared_ptr<FrameBuffer>& frameBuffer)
            : _frameBuffer(frameBuffer) {}

    const std::shared_ptr<FrameBuffer>&
    DefaultRenderPassStrategy::getFrameBuffer() const {
        return _frameBuffer;
    }

    void DefaultRenderPassStrategy::render(
            Room* room,
            const Render* render,
            const std::vector<std::shared_ptr<Material>>& materials) const {
        render->beginRenderPass(_frameBuffer);
        for (const auto& material: materials) {
            if (material->getTarget() != _frameBuffer) continue;
            for (const auto& [model, amount]: room->usedModels()) {
                model->draw(material);
            }
        }
        render->endRenderPass();
    }

    bool DefaultRenderPassStrategy::requiresRecreation() {
        return _frameBuffer->requiresRecreation();
    }

    void DefaultRenderPassStrategy::recreate() {
        _frameBuffer->recreate();
    }
}
