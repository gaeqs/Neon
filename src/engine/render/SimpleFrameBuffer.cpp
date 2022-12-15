//
// Created by grial on 15/12/22.
//

#include "SimpleFrameBuffer.h"

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
