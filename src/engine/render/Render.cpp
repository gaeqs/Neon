//
// Created by gaelr on 22/12/2022.
//

#include "Render.h"

#include <utility>

#include <engine/render/FrameBuffer.h>


namespace neon {
    Render::Render(Application* application,
                   std::string name,
                   const std::shared_ptr<ShaderUniformDescriptor>& descriptor) :
            Asset(typeid(Render), std::move(name)),
            _implementation(application),
            _application(application),
            _strategies(),
            _globalUniformDescriptor(descriptor),
            _globalUniformBuffer(name, descriptor) {
        _globalUniformBuffer.setBindingPoint(0);
    }

    const Render::Implementation& Render::getImplementation() const {
        return _implementation;
    }

    Render::Implementation& Render::getImplementation() {
        return _implementation;
    }

    void Render::addRenderPass(const RenderPassStrategy& strategy) {
        _strategies.push_back(strategy);
    }

    void Render::clearRenderPasses() {
        _strategies.clear();
    }

    void Render::render(Room* room) const {
        _implementation.render(room, _strategies);
    }

    void Render::checkFrameBufferRecreationConditions() {
        bool first = true;
        for (const auto& item: _strategies) {
            if (item.frameBuffer->requiresRecreation()) {
                if (first) {
                    _implementation.setupFrameBufferRecreation();
                    first = false;
                }
                item.frameBuffer->recreate();
            }
        }
    }

    size_t Render::getPassesAmount() const {
        return _strategies.size();
    }

    std::shared_ptr<FrameBuffer> Render::getFrameBuffer(size_t index) {
        return _strategies[index].frameBuffer;
    }

    const std::shared_ptr<ShaderUniformDescriptor>&
    Render::getGlobalUniformDescriptor() const {
        return _globalUniformDescriptor;
    }

    const ShaderUniformBuffer& Render::getGlobalUniformBuffer() const {
        return _globalUniformBuffer;
    }

    ShaderUniformBuffer& Render::getGlobalUniformBuffer() {
        return _globalUniformBuffer;
    }
}
