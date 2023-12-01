//
// Created by gaeqs on 01/12/2023.
//

#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include <vector>

#if USE_VULKAN

#include <vulkan/render/VKCommandPool.h>

#endif

namespace neon {
#if USE_VULKAN
    using Implementation = vulkan::VKCommandPool;
#endif

    class CommandBuffer;

    class CommandPool {
        std::vector<CommandBuffer> _buffers;
        std::vector<CommandBuffer> _usedBuffers;

        CommandBuffer& requestBuffer();
    };
}


#endif //COMMANDPOOL_H
