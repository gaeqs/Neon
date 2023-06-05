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

    class VKApplication;

    enum class VKCommandBufferStatus {
        CREATED,
        RECORDING,
        FINISHED
    };

    class VKCommandBuffer {

        VKApplication* _vkApplication;
        VkCommandBuffer _commandBuffer;

        VKCommandBufferStatus _status;

        std::vector<VkFence> _fences;
        std::vector<VkFence> _freedFences;

        void waitForFences();

        VkFence fetchFence();

    public:

        VKCommandBuffer(const VKCommandBuffer& other) = delete;

        VKCommandBuffer(Application* application, bool primary);

        VKCommandBuffer(VKApplication* application, bool primary);

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

    };
}


#endif //NEON_VKCOMMANDBUFFER_H
