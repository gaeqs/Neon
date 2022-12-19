//
// Created by grial on 15/12/22.
//

#include "RenderPass.h"

RenderPass::RenderPass(Application* application,
                       const FrameBuffer& frameBuffer) :
        _implementation(application, frameBuffer) {
}

const RenderPass::Implementation& RenderPass::getImplementation() const {
    return _implementation;
}

RenderPass::Implementation& RenderPass::getImplementation() {
    return _implementation;
}
