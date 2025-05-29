//
// Created by gaeqs on 17/04/2025.
//

#include "VKResource.h"

namespace neon::vulkan
{

    void VKResource::discardFinished()
    {
        std::erase_if(_runs, [](const std::shared_ptr<CommandBufferRun>& run) { return run->hasFinished(); });
    }

    VKResource::VKResource(AbstractVKApplication* application) :
        _application(application)
    {
    }

    VKResource::VKResource(Application* application) :
        _application(dynamic_cast<AbstractVKApplication*>(application->getImplementation()))
    {
    }

    AbstractVKApplication* VKResource::getApplication()
    {
        return _application;
    }

    const AbstractVKApplication* VKResource::getApplication() const
    {
        return _application;
    }

    void VKResource::registerRun(std::shared_ptr<CommandBufferRun> run)
    {
        if (run == nullptr) {
            return;
        }
        discardFinished();
        if (!run->hasFinished()) {
            _runs.push_back(std::move(run));
        }
    }

    std::vector<std::shared_ptr<CommandBufferRun>> VKResource::getRuns()
    {
        discardFinished();
        return _runs;
    }

    VkDevice VKResource::rawDevice() const
    {
        return _application->getDevice()->getRaw();
    }

    VkPhysicalDevice VKResource::rawPhysicalDevice() const
    {
        return _application->getPhysicalDevice().getRaw();
    }
} // namespace neon::vulkan