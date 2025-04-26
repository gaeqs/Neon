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

    VKResource::VKResource() = default;

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
} // namespace neon::vulkan