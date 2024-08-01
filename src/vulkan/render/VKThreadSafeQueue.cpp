//
// Created by gaelr on 16/01/2024.
//

#include "VKThreadSafeQueue.h"

VKThreadSafeQueue::VKThreadSafeQueue(VkQueue queue) : _queue(queue) {
}

void VKThreadSafeQueue::submit(VkSubmitInfo buffer, VkFence fence) {
    _mutex.lock();
    vkQueueSubmit(_queue, 1, &buffer, fence);
    _mutex.unlock();
}

VkQueue VKThreadSafeQueue::getQueue() const {
    return _queue;
}

void VKThreadSafeQueue::setQueue(VkQueue queue) {
    _queue = queue;
}
