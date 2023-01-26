//
// Created by gaelr on 22/12/2022.
//

#include "Render.h"

#include <engine/render/FrameBuffer.h>

Render::Render(Application* application) :
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

void Render::addRenderPass(RenderPassStrategy strategy) {
    _strategies.emplace(strategy);
}

void Render::clearRenderPasses() {
    _strategies.clear();
}

void Render::render(Room* room) const {
    _implementation.render(room, _strategies);
}

void Render::recreateFrameBuffers() {
    for (const auto& item: _strategies) {
        item.frameBuffer->recreate();
    }
}
