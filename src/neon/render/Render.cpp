//
// Created by gaelr on 22/12/2022.
//

#include "Render.h"

#include <utility>
#include <set>
#include <unordered_set>

#include <neon/render/buffer/FrameBuffer.h>
#include <neon/render/shader/Material.h>
#include <neon/structure/Room.h>
#include <neon/render/model/Model.h>

namespace neon
{
    Render::Render(Application* application, std::string name,
                   const std::shared_ptr<ShaderUniformDescriptor>& descriptor) :
        Asset(typeid(Render), name),
        _implementation(application),
        _application(application),
        _globalUniformDescriptor(descriptor),
        _globalUniformBuffer(std::move(name), descriptor)
    {
    }

    const Render::Implementation& Render::getImplementation() const
    {
        return _implementation;
    }

    Render::Implementation& Render::getImplementation()
    {
        return _implementation;
    }

    void Render::addRenderPass(const std::shared_ptr<RenderPassStrategy>& strategy)
    {
        _strategies.insert(strategy);
    }

    bool Render::removeRenderPass(const std::shared_ptr<RenderPassStrategy>& strategy)
    {
        return _strategies.erase(strategy) > 0;
    }

    void Render::clearRenderPasses()
    {
        _strategies.clear();
    }

    void Render::render(Room* room) const
    {
        if (!room) {
            return;
        }

        std::set<std::shared_ptr<Material>> materials;
        for (const auto& model : room->usedModels() | std::views::keys) {
            for (size_t i = 0; i < model->getMeshesAmount(); ++i) {
                for (const auto& material : model->getDrawable(i)->getMaterials()) {
                    materials.insert(material);
                }
            }
        }

        std::vector sortedMaterials(materials.begin(), materials.end());
        std::ranges::sort(sortedMaterials,
                          [](const auto& a, const auto& b) { return a->getPriority() > b->getPriority(); });

        for (const auto& strategy : _strategies) {
            strategy->render(room, this, sortedMaterials);
        }
    }

    void Render::checkFrameBufferRecreationConditions()
    {
        bool first = true;
        for (const auto& item : _strategies) {
            if (item->requiresRecreation()) {
                if (first) {
                    _implementation.setupFrameBufferRecreation();
                    first = false;
                }
                item->recreate();
            }
        }
    }

    size_t Render::getStrategyAmount() const
    {
        return _strategies.size();
    }

    const std::shared_ptr<ShaderUniformDescriptor>& Render::getGlobalUniformDescriptor() const
    {
        return _globalUniformDescriptor;
    }

    const ShaderUniformBuffer& Render::getGlobalUniformBuffer() const
    {
        return _globalUniformBuffer;
    }

    ShaderUniformBuffer& Render::getGlobalUniformBuffer()
    {
        return _globalUniformBuffer;
    }

    void Render::beginRenderPass(const std::shared_ptr<FrameBuffer>& fb, bool clear) const
    {
        _implementation.beginRenderPass(fb, clear);
    }

    void Render::endRenderPass() const
    {
        _implementation.endRenderPass();
    }
} // namespace neon
