//
// Created by gaelr on 22/12/2022.
//

#include "Render.h"

#include <utility>

#include <engine/render/FrameBuffer.h>


namespace neon {
    Render::Render(Application* application, std::string name) :
            Asset(typeid(Render), std::move(name)),
            _implementation(application),
            _application(application),
            _strategies() {

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
}
