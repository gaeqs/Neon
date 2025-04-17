//
// Created by gaelr on 4/06/23.
//

#ifndef NEON_COMMANDBUFFER_H
#define NEON_COMMANDBUFFER_H

#ifdef USE_VULKAN

    #include <vulkan/render/VKCommandBuffer.h>
    #include <vulkan/render/VKCommandPool.h>

#endif
#include "CommandBufferRun.h"

namespace neon
{
    class Application;

    class CommandBuffer
    {
      public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKCommandBuffer;
#endif

      private:
        Implementation _implementation;

      public:
        CommandBuffer(const CommandBuffer& other) = delete;

        CommandBuffer(CommandBuffer&& move) noexcept;

        CommandBuffer(Application* application, bool primary);

#ifdef USE_VULKAN

        CommandBuffer(const vulkan::VKCommandPool& pool, bool primary);

        CommandBuffer(Application* application, VkCommandBuffer commandBuffer, VkQueue queue);

#endif

        [[nodiscard]] const Implementation& getImplementation() const;

        [[nodiscard]] Implementation& getImplementation();

        [[nodiscard]] std::shared_ptr<CommandBufferRun> getCurrentRun() const;

        bool begin(bool onlyOneSubmit = false);

        bool end();

        bool submit();

        void wait();

        void reset(bool releaseResources = true);

        bool isBeingUsed() const;

        CommandBuffer& operator=(CommandBuffer&& move) noexcept;
    };
} // namespace neon

#endif // NEON_COMMANDBUFFER_H
