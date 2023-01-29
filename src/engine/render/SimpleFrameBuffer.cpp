//
// Created by grial on 15/12/22.
//

#include "SimpleFrameBuffer.h"


bool SimpleFrameBuffer::defaultRecreationCondition(
        const SimpleFrameBuffer* fb) {
    return fb->_implementation.defaultRecreationCondition();
}

std::pair<uint32_t, uint32_t>
SimpleFrameBuffer::defaultRecreationParameters(const SimpleFrameBuffer* fb) {
    return fb->_implementation.defaultRecreationParameters();
}

SimpleFrameBuffer::SimpleFrameBuffer(
        Room* room,
        const std::vector<TextureFormat>& colorFormats,
        bool depth) :
        _implementation(room, colorFormats, depth),
        _recreationCondition(defaultRecreationCondition),
        _recreationParameters(defaultRecreationParameters) {

}

bool SimpleFrameBuffer::requiresRecreation() {
    return _recreationCondition(this);
}

void SimpleFrameBuffer::recreate() {
    _implementation.recreate(_recreationParameters(this));
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

const std::function<bool(const SimpleFrameBuffer*)>&
SimpleFrameBuffer::getRecreationCondition() const {
    return _recreationCondition;
}

void SimpleFrameBuffer::setRecreationCondition(const std::function<bool(
        const SimpleFrameBuffer*)>& recreationCondition) {
    _recreationCondition = recreationCondition;
}

const std::function<std::pair<uint32_t, uint32_t>(const SimpleFrameBuffer*)>&
SimpleFrameBuffer::getRecreationParameters() const {
    return _recreationParameters;
}

void SimpleFrameBuffer::setRecreationParameters(
        const std::function<std::pair<uint32_t, uint32_t>(
                const SimpleFrameBuffer*)>& recreationParameters) {
    _recreationParameters = recreationParameters;
}
