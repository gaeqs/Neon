//
// Created by gaeqs on 16/04/2025.
//

#include "VKResourceBin.h"

#include <neon/logging/Logger.h>

namespace neon
{
    VKResourceBin::~VKResourceBin()
    {
        waitAndFlush();
    }

    void VKResourceBin::destroyLater(VkDevice device, std::vector<std::shared_ptr<CommandBufferRun>> runs,
                                     std::function<void()> deleteFunction)
    {
        std::lock_guard lock(_mutex);
        _entries.emplace_back(device, std::move(runs), std::move(deleteFunction));
    }

    void VKResourceBin::flush()
    {
        std::lock_guard lock(_mutex);

        std::erase_if(_entries, [](const Entry& entry) {
            bool finished = std::ranges::all_of(entry.runs, [](auto& run) { return run->hasFinished(); });
            if (finished) {
                entry.deleteFunction();
            }
            return finished;
        });
    }

    void VKResourceBin::waitAndFlush()
    {
        for (auto& entry : _entries) {
            for (auto& run : entry.runs) {
                run->wait();
            }
            entry.deleteFunction();
        }
        _entries.clear();
    }
} // namespace neon