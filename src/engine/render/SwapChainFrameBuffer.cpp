//
// Created by gaelr on 14/12/2022.
//

#include "SwapChainFrameBuffer.h"

SwapChainFrameBuffer::SwapChainFrameBuffer(
        Room* room, bool depth) :
        FrameBuffer(),
        _implementation(room, depth) {

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

std::vector<IdentifiableWrapper<Texture>>
SwapChainFrameBuffer::getTextures() const {
    return {};
}
