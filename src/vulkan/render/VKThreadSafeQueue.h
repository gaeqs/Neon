//
// Created by gaelr on 16/01/2024.
//

#ifndef VKTHREADSAFEQUEUE_H
#define VKTHREADSAFEQUEUE_H

#include <mutex>
#include <vulkan/vulkan_core.h>


class VKThreadSafeQueue {
    VkQueue _queue;
    std::mutex _mutex;

public:
    explicit VKThreadSafeQueue(VkQueue queue);

    void submit(VkSubmitInfo submitInfo, VkFence fence);

    [[nodiscard]] VkQueue getQueue() const;

    void setQueue(VkQueue queue);
};


#endif //VKTHREADSAFEQUEUE_H
