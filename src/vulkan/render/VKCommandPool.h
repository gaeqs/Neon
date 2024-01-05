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
        explicit VKCommandPool(Application* application);

        ~VKCommandPool();

        [[nodiscard]] VkCommandPool raw() const;

        [[nodiscard]] std::unique_ptr<CommandBuffer> newCommandBuffer(
            bool primary) const;
    };
}


#endif //VKCOMMANDPOOL_H
