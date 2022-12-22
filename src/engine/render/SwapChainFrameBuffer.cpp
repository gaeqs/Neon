//
// Created by gaelr on 14/12/2022.
//

#include "SwapChainFrameBuffer.h"

SwapChainFrameBuffer::SwapChainFrameBuffer(
        Application* application, bool depth) :
        FrameBuffer(),
        _implementation(application, depth) {

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
