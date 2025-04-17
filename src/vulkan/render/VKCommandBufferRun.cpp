//
// Created by gaeqs on 16/04/2025.
//

#include "VKCommandBufferRun.h"

#include "VKCommandBuffer.h"

namespace neon::vulkan
{

    VKCommandBufferRun::VKCommandBufferRun(VKCommandBuffer* cmd) :
        _cmd(cmd)
    {
    }

    bool VKCommandBufferRun::hasFinished()
    {
        if (_cmd == nullptr) {
            return true;
        }

        if (!_cmd->isBeingUsed()) {
            _cmd = nullptr;
            return true;
        }

        return false;
    }

    void VKCommandBufferRun::markCompletion()
    {
        _cmd = nullptr;
    }
} // namespace neon::vulkan