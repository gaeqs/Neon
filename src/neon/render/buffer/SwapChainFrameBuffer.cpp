//
// Created by gaelr on 14/12/2022.
//

#include "SwapChainFrameBuffer.h"

namespace neon
{
    SwapChainFrameBuffer::SwapChainFrameBuffer(Application* application, std::string name, SamplesPerTexel samples,
                                               bool depth) :
        FrameBuffer(std::move(name)),
        _implementation(application, samples, depth)
    {
    }

    bool SwapChainFrameBuffer::requiresRecreation()
    {
        return _implementation.requiresRecreation();
    }

    void SwapChainFrameBuffer::recreate()
    {
        _implementation.recreate();
    }

    FrameBuffer::Implementation& SwapChainFrameBuffer::getImplementation()
    {
        return _implementation;
    }

    const FrameBuffer::Implementation& SwapChainFrameBuffer::getImplementation() const
    {
        return _implementation;
    }

    rush::Vec2ui SwapChainFrameBuffer::getDimensions() const
    {
        return _implementation.getDimensions();
    }

    std::vector<FrameBufferOutput> SwapChainFrameBuffer::getOutputs() const
    {
        return {};
    }

    SamplesPerTexel SwapChainFrameBuffer::getSamples() const
    {
        return _implementation.getSamples();
    }
} // namespace neon
