//
// Created by gaelr on 14/12/2022.
//

#ifdef USE_QT

    #include "QTSwapChainFrameBuffer.h"

namespace neon
{
    QTSwapChainFrameBuffer::QTSwapChainFrameBuffer(Application* application) :
        FrameBuffer(),
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

    std::vector<std::shared_ptr<Texture>> QTSwapChainFrameBuffer::getTextures() const
    {
        return {};
    }

    uint32_t QTSwapChainFrameBuffer::getWidth() const
    {
        return _implementation.getWidth();
    }

    uint32_t QTSwapChainFrameBuffer::getHeight() const
    {
        return _implementation.getHeight();
    }
} // namespace neon

#endif