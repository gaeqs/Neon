//
// Created by grial on 15/12/22.
//

#include "SimpleFrameBuffer.h"

SimpleFrameBuffer::SimpleFrameBuffer(
        Room* room,
        std::vector<TextureFormat> colorFormats,
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
