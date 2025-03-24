//
// Created by gaelr on 22/12/2022.
//

#include "Render.h"

#include <utility>
#include <queue>
#include <unordered_set>

#include <neon/render/buffer/FrameBuffer.h>
#include <neon/render/shader/Material.h>
#include <neon/structure/Room.h>
#include <neon/render/model/Model.h>

namespace neon
{
    Render::Render(Application* application, std::string name,
                   const std::shared_ptr<ShaderUniformDescriptor>& descriptor) :
        Asset(typeid(Render), std::move(name)),
        _implementation(application),
        _application(application),
        _globalUniformDescriptor(descriptor),
        _globalUniformBuffer(name, descriptor)
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
        _strategies.push_back(strategy);
    }

    void Render::clearRenderPasses()
    {
        _strategies.clear();
    }

    void Render::render(Room* room) const
    {
        // Get the material priority list
        struct CustomLess
        {
            bool operator()(const std::shared_ptr<Material>& l, const std::shared_ptr<Material>& r) const
            {
                return l->getPriority() < r->getPriority();
            }
        };

        std::priority_queue<std::shared_ptr<Material>, std::vector<std::shared_ptr<Material>>, CustomLess> queue;

        std::unordered_set<std::shared_ptr<Material>> materials;
        if (room != nullptr) {
            for (const auto& [model, _] : room->usedModels()) {
                for (int i = 0; i < model->getMeshesAmount(); ++i) {
                    const auto& mesh = model->getDrawable(i);
                    for (const auto& material : mesh->getMaterials()) {
                        materials.insert(material);
                    }
                }
            }
        }

        for (const auto& material : materials) {
            queue.push(material);
        }

        std::vector<std::shared_ptr<Material>> vector;
        vector.reserve(queue.size());
        while (!queue.empty()) {
            vector.push_back(queue.top());
            queue.pop();
        }

        _implementation.render(room, this, vector, _strategies);
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

    const std::vector<std::shared_ptr<RenderPassStrategy>>& Render::getStrategies() const
    {
        return _strategies;
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
