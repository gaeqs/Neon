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

    class AbstractVKApplication;

    class VKResource
    {
        AbstractVKApplication* _application;
        std::vector<std::shared_ptr<CommandBufferRun>> _runs;

        void discardFinished();

      public:
        explicit VKResource(AbstractVKApplication* application);

        explicit VKResource(Application* application);

        [[nodiscard]] AbstractVKApplication* getApplication();

        [[nodiscard]] const AbstractVKApplication* getApplication() const;

        void registerRun(std::shared_ptr<CommandBufferRun> run);

        [[nodiscard]] std::vector<std::shared_ptr<CommandBufferRun>> getRuns();

        [[nodiscard]] VkDevice rawDevice() const;

        [[nodiscard]] VkPhysicalDevice rawPhysicalDevice() const;
    };
} // namespace neon::vulkan

#endif // VKRESOURCE_H
