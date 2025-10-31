//
// Created by gaeqs on 17/04/2025.
//

#include "VKResource.h"

#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan
{

    void VKResource::discardFinished()
    {
        std::erase_if(_runs, [](const std::shared_ptr<CommandBufferRun>& run) { return run->hasFinished(); });
    }

    VKResource::VKResource(AbstractVKApplication* application) :
        _application(application),
        _registeredRunsHistory(0)
    {
    }

    VKResource::VKResource(Application* application) :
        _application(dynamic_cast<AbstractVKApplication*>(application->getImplementation())),
        _registeredRunsHistory(0)
    {
    }

    AbstractVKApplication* VKResource::getApplication() const
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
            ++_registeredRunsHistory;
        }
    }

    std::vector<std::shared_ptr<CommandBufferRun>> VKResource::getRuns()
    {
        discardFinished();
        return _runs;
    }

    VKDeviceHolder VKResource::holdRawDevice() const
    {
        return _application->getDevice()->hold();
    }

    VkPhysicalDevice VKResource::rawPhysicalDevice() const
    {
        return _application->getPhysicalDevice().getRaw();
    }

    size_t VKResource::getRegisteredRunsHistory() const
    {
        return _registeredRunsHistory;
    }
} // namespace neon::vulkan