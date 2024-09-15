//
// Created by gaeqs on 01/12/2023.
//

#ifndef VKCOMMANDPOOL_H
#define VKCOMMANDPOOL_H

#include <memory>

#include <vulkan/vulkan.h>
#include <vulkan/queue/VKQueueFamily.h>

namespace neon {
    class CommandBuffer;
    class Application;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKCommandPool {
        Application* _application;
        AbstractVKApplication* _vkApplication;
        VkCommandPool _raw;
        uint32_t _queueFamilyIndex;

        bool _external;

    public:
        VKCommandPool(const VKCommandPool& other) = delete;

        VKCommandPool(VKCommandPool&& move) noexcept;

        explicit VKCommandPool(Application* application,
                               VKQueueFamily::Capabilities capabilities =
                                       VKQueueFamily::Capabilities::withGraphics());

        explicit VKCommandPool(Application* application,
                               VkCommandPool external,
                               uint32_t externalQueueFamilyIndex);

        ~VKCommandPool();

        [[nodiscard]] AbstractVKApplication* getVkApplication() const;

        [[nodiscard]] uint32_t getQueueFamilyIndex() const;

        [[nodiscard]] VkCommandPool raw() const;

        [[nodiscard]] std::unique_ptr<CommandBuffer> newCommandBuffer(
            bool primary) const;

        VKCommandPool& operator=(VKCommandPool&& move) noexcept;
    };
}


#endif //VKCOMMANDPOOL_H
