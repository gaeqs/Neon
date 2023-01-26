//
// Created by grial on 15/12/22.
//

#include "SimpleFrameBuffer.h"

SimpleFrameBuffer::SimpleFrameBuffer(
        Room* room,
        const std::vector<TextureFormat>& colorFormats,
        bool depth) :
        _implementation(room, colorFormats, depth) {

}

void SimpleFrameBuffer::recreate() {
    _implementation.recreate();
}

FrameBuffer::Implementation& SimpleFrameBuffer::getImplementation() {
    return _implementation;
}

const FrameBuffer::Implementation&
SimpleFrameBuffer::getImplementation() const {
    return _implementation;
}

std::vector<IdentifiableWrapper<Texture>>
SimpleFrameBuffer::getTextures() const {
    return _implementation.getTextures();
}

ImTextureID SimpleFrameBuffer::getImGuiDescriptor(uint32_t index) {
    return _implementation.getImGuiDescriptor(index);
}

uint32_t SimpleFrameBuffer::getWidth() const {
    return _implementation.getWidth();
}

uint32_t SimpleFrameBuffer::getHeight() const {
    return _implementation.getHeight();
}
