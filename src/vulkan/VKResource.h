//
// Created by gaeqs on 17/04/2025.
//

#ifndef VKRESOURCE_H
#define VKRESOURCE_H
#include "VKApplication.h"

#include <memory>
#include <vector>
#include <neon/render/buffer/CommandBufferRun.h>

namespace neon::vulkan
{
    class VKResource
    {
        std::vector<std::shared_ptr<CommandBufferRun>> _runs;

        void discardFinished();

      public:
        VKResource();

        void registerRun(std::shared_ptr<CommandBufferRun> run);

        std::vector<std::shared_ptr<CommandBufferRun>> getRuns();
    };
} // namespace neon::vulkan

#endif // VKRESOURCE_H
