//
// Created by gaelr on 14/12/2022.
//

#ifdef USE_QT

    #include "QTSwapChainFrameBuffer.h"

namespace neon
{
    QTSwapChainFrameBuffer::QTSwapChainFrameBuffer(Application* application, std::string name) :
        FrameBuffer(std::move(name)),
        _implementation(application)
    {
    }

    bool QTSwapChainFrameBuffer::requiresRecreation()
    {
        return false;
    }

    void QTSwapChainFrameBuffer::recreate()
    {
    }

    FrameBuffer::Implementation& QTSwapChainFrameBuffer::getImplementation()
    {
        return _implementation;
    }

    const FrameBuffer::Implementation& QTSwapChainFrameBuffer::getImplementation() const
    {
        return _implementation;
    }

    rush::Vec2ui QTSwapChainFrameBuffer::getDimensions() const
    {
        return _implementation.getDimensions();
    }

    std::vector<FrameBufferOutput> QTSwapChainFrameBuffer::getOutputs() const
    {
        return _implementation.getOutputs();
    }

    SamplesPerTexel QTSwapChainFrameBuffer::getSamples() const
    {
        return _implementation.getSamples();
    }
} // namespace neon

#endif