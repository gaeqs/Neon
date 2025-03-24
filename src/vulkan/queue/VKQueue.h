//
// Created by gaeqs on 4/08/24.
//

#ifndef VKQUEUE_H
#define VKQUEUE_H

#include <vulkan/vulkan.h>

namespace neon::vulkan
{
    class VKQueue
    {
        VkQueue _raw;

      public:
        VKQueue(VkDevice device, uint32_t familyIndex, uint32_t queueIndex);

        [[nodiscard]] VkQueue getRaw() const;
    };
} // namespace neon::vulkan

#endif //VKQUEUE_H
