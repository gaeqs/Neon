//
// Created by gaeqs on 4/08/24.
//

#include "VKQueue.h"

neon::vulkan::VKQueue::VKQueue(VkDevice device,
                               uint32_t familyIndex,
                               uint32_t queueIndex) {
    vkGetDeviceQueue(device, familyIndex, queueIndex, &_raw);
}

VkQueue neon::vulkan::VKQueue::getRaw() const {
    return _raw;
}
