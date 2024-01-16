//
// Created by gaeqs on 01/12/2023.
//

#ifndef VKCOMMANDPOOL_H
#define VKCOMMANDPOOL_H

#include <memory>

#include <vulkan/vulkan.h>

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

    public:
        VKCommandPool(const VKCommandPool& other) = delete;

        VKCommandPool(VKCommandPool&& move) noexcept;

        explicit VKCommandPool(Application* application);

        ~VKCommandPool();

        [[nodiscard]] VkCommandPool raw() const;

        [[nodiscard]] std::unique_ptr<CommandBuffer> newCommandBuffer(
            bool primary) const;

        VKCommandPool& operator=(VKCommandPool&& move) noexcept;
    };
}


#endif //VKCOMMANDPOOL_H
