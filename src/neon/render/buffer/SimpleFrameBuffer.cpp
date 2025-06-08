//
// Created by grial on 15/12/22.
//

#include "SimpleFrameBuffer.h"

#include <neon/structure/Application.h>

#include <utility>

namespace neon
{
    bool SimpleFrameBuffer::defaultRecreationCondition(const SimpleFrameBuffer* fb)
    {
        auto vp = fb->_application->getViewport();
        if (vp.x() == 0 || vp.y() == 0) {
            return false;
        }
        return fb->getDimensions().cast<int32_t>() != vp;
    }

    rush::Vec2ui SimpleFrameBuffer::defaultRecreationParameters(Application* app)
    {
        auto vp = app->getViewport();
        return {std::max(vp.x(), 1), std::max(vp.y(), 1)};
    }

    SimpleFrameBuffer::SimpleFrameBuffer(Application* application, std::string name, SamplesPerTexel samples,
                                         const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                                         const std::optional<FrameBufferDepthCreateInfo>& depthInfo,
                                         Condition condition, Parameters parameters) :
        FrameBuffer(std::move(name)),
        _application(application),
        _implementation(application, parameters(application), samples, textureInfos, depthInfo),
        _recreationCondition(std::move(condition)),
        _recreationParameters(std::move(parameters))
    {
    }

    bool SimpleFrameBuffer::requiresRecreation()
    {
        return _recreationCondition(this);
    }

    void SimpleFrameBuffer::recreate()
    {
        _implementation.recreate(_recreationParameters(_application));
    }

    FrameBuffer::Implementation& SimpleFrameBuffer::getImplementation()
    {
        return _implementation;
    }

    const FrameBuffer::Implementation& SimpleFrameBuffer::getImplementation() const
    {
        return _implementation;
    }

    std::vector<FrameBufferOutput> SimpleFrameBuffer::getOutputs() const
    {
        return _implementation.getOutputs();
    }

    rush::Vec2ui SimpleFrameBuffer::getDimensions() const
    {
        return _implementation.getDimensions();
    }

    SamplesPerTexel SimpleFrameBuffer::getSamples() const
    {
        return _implementation.getSamples();
    }

    const SimpleFrameBuffer::Condition& SimpleFrameBuffer::getRecreationCondition() const
    {
        return _recreationCondition;
    }

    void SimpleFrameBuffer::setRecreationCondition(const Condition& recreationCondition)
    {
        _recreationCondition = recreationCondition;
    }

    const SimpleFrameBuffer::Parameters& SimpleFrameBuffer::getRecreationParameters() const
    {
        return _recreationParameters;
    }

    void SimpleFrameBuffer::setRecreationParameters(const Parameters& recreationParameters)
    {
        _recreationParameters = recreationParameters;
    }
} // namespace neon
