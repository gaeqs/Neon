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
        std::vector<std::unique_ptr<CommandBuffer>> _buffers;
        std::vector<size_t> _availableBuffers;
        std::vector<size_t> _usedBuffers;

        void checkUsedBufferForAvailability();

    public:

        explicit CommandPool(Application* application);

        CommandBuffer* beginCommandBuffer(bool onlyOneSummit = false);
    };
}


#endif //COMMANDPOOL_H
