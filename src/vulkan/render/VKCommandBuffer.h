//
// Created by gaelr on 4/06/23.
//

#ifndef NEON_VKCOMMANDBUFFER_H
#define NEON_VKCOMMANDBUFFER_H

#include "VKCommandBufferRun.h"

#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/queue/VKQueueProvider.h>
#include <vulkan/render/VKCommandPool.h>
#include <vulkan/sync/VKSemaphore.h>

namespace neon
{
    class Application;
}

namespace neon::vulkan
{
    class AbstractVKApplication;

    enum class VKCommandBufferStatus
    {
        READY,
        RECORDING,
        RECORDED
    };

    class VKCommandBuffer
    {
        AbstractVKApplication* _vkApplication;
        VkCommandPool _pool;
        VKQueueHolder _queueHolder;
        VkQueue _queue;
        VkCommandBuffer _commandBuffer;

        VKCommandBufferStatus _status;

        mutable std::vector<VkFence> _fences;
        mutable std::vector<VkFence> _freedFences;

        std::shared_ptr<VKCommandBufferRun> _currentRun;

        bool _external;

        void refreshStatus();

        VkFence fetchAvailableFence();

        VkFence createFence() const;

        void printInvalidState(VKCommandBufferStatus expected) const;

        void free();

      public:
        VKCommandBuffer(const VKCommandBuffer& other) = delete;

        VKCommandBuffer(VKCommandBuffer&& move) noexcept;

        VKCommandBuffer(Application* application, bool primary);

        VKCommandBuffer(const VKCommandPool& pool, bool primary);

        VKCommandBuffer(Application* application, VkCommandBuffer commandBuffer, VkQueue queue);

        ~VKCommandBuffer();

        [[nodiscard]] VkCommandBuffer getCommandBuffer() const;

        [[nodiscard]] std::shared_ptr<CommandBufferRun> getCurrentRun() const;

        void waitForFences();

        bool begin(bool onlyOneSubmit = false);

        bool end();

        bool submit(std::shared_ptr<VKSemaphore> waitSemaphore = nullptr,
                    std::shared_ptr<VKSemaphore> signalSemaphore = nullptr, VkPipelineStageFlags* waitStages = nullptr);

        void reset(bool releaseResources = true);

        bool isBeingUsed() const;

        /**
         * This method returns a new VkFence.
         * This fence will be registered into this command buffer, and
         * this command buffer won't be marked as finished until
         * this new VkFence is signaled.
         *
         * This is useful if you don't want to mark this command buffer
         * as finished until something else is finished, such as a presentation action.
         */
        VkFence createAndRegisterFence();

        VKCommandBuffer& operator=(VKCommandBuffer&& move) noexcept;
    };
} // namespace neon::vulkan

#endif // NEON_VKCOMMANDBUFFER_H
