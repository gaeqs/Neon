//
// Created by gaelr on 25/12/2022.
//

#include "RenderPassStrategy.h"

#include <engine/model/Model.h>
#include <engine/structure/Room.h>
#include <engine/render/FrameBuffer.h>
#include <engine/shader/Material.h>

namespace neon {
    RenderPassStrategy::RenderPassStrategy(
            const std::shared_ptr<FrameBuffer>& _frameBuffer,
            const std::function<void(
                    Room*,
                    const std::vector<std::shared_ptr<Material>>&,
                    std::shared_ptr<FrameBuffer>)>& _strategy) :
            frameBuffer(_frameBuffer),
            strategy(_strategy) {

    }

    void RenderPassStrategy::defaultStrategy(
            Room* room,
            const std::vector<std::shared_ptr<Material>>& sortedMaterials,
            const std::shared_ptr<FrameBuffer>& target) {
        for (const auto& material: sortedMaterials) {
            if (material->getTarget() != target) continue;
            for (const auto& [model, amount]: room->usedModels()) {
                model->draw(material);
            }
        }
    }
}
