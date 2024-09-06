//
// Created by gaelr on 14/12/2022.
//

#include "FrameBuffer.h"

namespace neon {

    FrameBuffer::FrameBuffer() : _clearColors(), _depthClear(1.0f, 0) {

    }

    std::optional<rush::Vec4f>
    FrameBuffer::getClearColor(uint32_t index) const {
        auto it = _clearColors.find(index);
        if (it != _clearColors.end()) return it->second;
        return {};
    }

    void FrameBuffer::setClearColor(uint32_t index, rush::Vec4f color) {
        _clearColors[index] = color;
    }

    std::pair<float, uint32_t> FrameBuffer::getDepthClearColor() const {
        return _depthClear;
    }

    void FrameBuffer::setDepthClearColor(float depth, uint32_t stencil) {
        _depthClear = {depth, stencil};
    }

}