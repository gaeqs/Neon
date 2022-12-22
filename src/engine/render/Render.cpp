//
// Created by gaelr on 22/12/2022.
//

#include "Render.h"

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

void Render::addRenderPass(const std::shared_ptr<FrameBuffer>& frameBuffer,
                           const std::function<void(Room*)>& strategy) {
    _strategies.emplace(_application, frameBuffer, strategy);
}

void Render::clearRenderPasses() {
    _strategies.clear();
}

void Render::render(Room* room) const {
    _implementation.render(room, _strategies);
}
