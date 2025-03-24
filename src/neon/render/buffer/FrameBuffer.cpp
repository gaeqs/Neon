//
// Created by gaelr on 14/12/2022.
//

#include "FrameBuffer.h"

namespace neon
{
    FrameBuffer::FrameBuffer(std::string name) :
        Asset(typeid(FrameBuffer), std::move(name)),
        _depthClear(1.0f, 0)
    {
    }

    std::optional<rush::Vec4f> FrameBuffer::getClearColor(uint32_t index) const
    {
        auto it = _clearColors.find(index);
        if (it != _clearColors.end()) {
            return it->second;
        }
        return {};
    }

    void FrameBuffer::setClearColor(uint32_t index, rush::Vec4f color)
    {
        _clearColors[index] = color;
    }

    std::pair<float, uint32_t> FrameBuffer::getDepthClearColor() const
    {
        return _depthClear;
    }

    void FrameBuffer::setDepthClearColor(float depth, uint32_t stencil)
    {
        _depthClear = {depth, stencil};
    }

    std::vector<std::shared_ptr<Texture>> FrameBuffer::getTextures() const
    {
        auto outputs = getOutputs();
        std::vector<std::shared_ptr<Texture>> textures;
        textures.reserve(outputs.size());

        for (auto& output : outputs) {
            textures.push_back(output.resolvedTexture);
        }

        return textures;
    }
} // namespace neon
