//
// Created by gaelr on 14/12/2022.
//

#include "SwapChainFrameBuffer.h"

namespace neon {
    SwapChainFrameBuffer::SwapChainFrameBuffer(
        Application* application, std::string name, SamplesPerTexel samples, bool depth) :
        FrameBuffer(std::move(name)),
        _implementation(application, samples, depth) {}

    bool SwapChainFrameBuffer::requiresRecreation() {
        return _implementation.requiresRecreation();
    }

    void SwapChainFrameBuffer::recreate() {
        _implementation.recreate();
    }

    FrameBuffer::Implementation& SwapChainFrameBuffer::getImplementation() {
        return _implementation;
    }

    const FrameBuffer::Implementation&
    SwapChainFrameBuffer::getImplementation() const {
        return _implementation;
    }

    std::vector<std::shared_ptr<Texture>>
    SwapChainFrameBuffer::getTextures() const {
        return {};
    }

    uint32_t SwapChainFrameBuffer::getWidth() const {
        return _implementation.getWidth();
    }

    uint32_t SwapChainFrameBuffer::getHeight() const {
        return _implementation.getHeight();
    }

    SamplesPerTexel SwapChainFrameBuffer::getSamples() const {
        return _implementation.getSamples();
    }
}
