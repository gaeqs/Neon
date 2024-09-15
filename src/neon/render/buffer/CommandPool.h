//
// Created by gaeqs on 01/12/2023.
//

#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include <vector>
#include <memory>

#if USE_VULKAN

#include <vulkan/render/VKCommandPool.h>

#endif

namespace neon {
#if USE_VULKAN
    using Implementation = vulkan::VKCommandPool;
#endif

    class Application;

    class CommandBuffer;

    class CommandPool {
        Implementation _implementation;

        std::vector<std::unique_ptr<CommandBuffer>> _buffers;
        std::vector<size_t> _availableBuffers;
        std::vector<size_t> _usedBuffers;

        void checkUsedBufferForAvailability();

    public:
        CommandPool(const CommandPool& other) = delete;

        CommandPool(CommandPool&& move) noexcept;

        explicit CommandPool(Application* application);

#ifdef USE_VULKAN

        CommandPool(Application* application,
                    VkCommandPool pool,
                    uint32_t queueFamilyIndex);

#endif

        ~CommandPool();

        Implementation& getImplementation();

        const Implementation& getImplementation() const;

        CommandBuffer* beginCommandBuffer(bool onlyOneSummit);

        CommandPool& operator=(CommandPool&& move) noexcept;
    };
}


#endif //COMMANDPOOL_H
