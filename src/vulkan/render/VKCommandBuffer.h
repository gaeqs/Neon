//
// Created by gaelr on 4/06/23.
//

#ifndef NEON_VKCOMMANDBUFFER_H
#define NEON_VKCOMMANDBUFFER_H

#include <vector>

#include <vulkan/vulkan.h>

namespace neon {
    class Application;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    enum class VKCommandBufferStatus {
        READY,
        RECORDING,
        RECORDED
    };

    class VKCommandBuffer {
        AbstractVKApplication* _vkApplication;
        VkCommandBuffer _commandBuffer;

        VKCommandBufferStatus _status;

        std::vector<VkFence> _fences;
        std::vector<VkFence> _freedFences;
        bool _external;

        void waitForFences();

        void refreshStatus();

        VkFence fetchAvailableFence();

        VkFence createFence() const;

    public:
        VKCommandBuffer(const VKCommandBuffer& other) = delete;

        VKCommandBuffer(Application* application, bool primary);

        VKCommandBuffer(Application* application,
                        VkCommandBuffer commandBuffer);

        ~VKCommandBuffer();

        [[nodiscard]] VkCommandBuffer getCommandBuffer() const;

        bool begin(bool onlyOneSubmit = false);

        bool end();

        bool submit(uint32_t waitSemaphoreAmount = 0,
                    VkSemaphore* waitSemaphores = nullptr,
                    VkPipelineStageFlags* waitStages = nullptr,
                    uint32_t signalSemaphoreAmount = 0,
                    VkSemaphore* signalSemaphores = nullptr);

        void reset(bool releaseResources = true);

        bool isBeingUsed();
    };
}


#endif //NEON_VKCOMMANDBUFFER_H
