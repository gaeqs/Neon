//
// Created by gaeqs on 17/04/2025.
//

#ifndef VKRESOURCE_H
#define VKRESOURCE_H

#include <neon/structure/Application.h>
#include <vulkan/vulkan.h>

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
        size_t _registeredRunsHistory;

        void discardFinished();

      public:
        explicit VKResource(AbstractVKApplication* application);

        explicit VKResource(Application* application);

        [[nodiscard]] AbstractVKApplication* getApplication() const;

        void registerRun(std::shared_ptr<CommandBufferRun> run);

        [[nodiscard]] std::vector<std::shared_ptr<CommandBufferRun>> getRuns();

        [[nodiscard]] VkDevice rawDevice() const;

        [[nodiscard]] VkPhysicalDevice rawPhysicalDevice() const;

        [[nodiscard]] size_t getRegisteredRunsHistory() const;
    };
} // namespace neon::vulkan

#endif // VKRESOURCE_H
