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
        return vp.x() != fb->getWidth() || vp.y() != fb->getHeight();
    }

    std::pair<uint32_t, uint32_t> SimpleFrameBuffer::defaultRecreationParameters(Application* app)
    {
        auto vp = app->getViewport();
        return {std::max(vp.x(), 1), std::max(vp.y(), 1)};
    }

    SimpleFrameBuffer::SimpleFrameBuffer(Application* application, std::string name,
                                         const std::vector<FrameBufferTextureCreateInfo>& textureInfos, bool depth,
                                         std::optional<std::string> depthName, SamplesPerTexel depthSamples,
                                         Condition condition, const Parameters& parameters) :
        FrameBuffer(std::move(name)),
        _application(application),
        _implementation(application, textureInfos, parameters(application), depth, std::move(depthName), depthSamples),
        _recreationCondition(std::move(condition)),
        _recreationParameters(parameters)
    {
    }

    SimpleFrameBuffer::SimpleFrameBuffer(Application* application, std::string name,
                                         const std::vector<FrameBufferTextureCreateInfo>& textureInfos,
                                         std::shared_ptr<Texture> depthTexture, Condition condition,
                                         const Parameters& parameters) :
        FrameBuffer(std::move(name)),
        _application(application),
        _implementation(application, textureInfos, parameters(application), std::move(depthTexture)),
        _recreationCondition(std::move(condition)),
        _recreationParameters(parameters)
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

    ImTextureID SimpleFrameBuffer::getImGuiDescriptor(uint32_t index)
    {
        return _implementation.getImGuiDescriptor(index);
    }

    uint32_t SimpleFrameBuffer::getWidth() const
    {
        return _implementation.getWidth();
    }

    uint32_t SimpleFrameBuffer::getHeight() const
    {
        return _implementation.getHeight();
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
