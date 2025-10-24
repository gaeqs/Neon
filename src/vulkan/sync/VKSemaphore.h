//
// Created by gaeqs on 20/10/25.
//

#ifndef NEON_VKSEMAPHORE_H
#define NEON_VKSEMAPHORE_H

#include <vulkan/VKResource.h>

namespace neon::vulkan
{
    class VKSemaphore : public VKResource
    {
        VkSemaphore _semaphore;

      public:
        VKSemaphore(const VKSemaphore& other) = delete;

        explicit VKSemaphore(Application* application);

        ~VKSemaphore();

        [[nodiscard]] VkSemaphore getRaw() const;
    };
} // namespace neon::vulkan

#endif // NEON_VKSEMAPHORE_H
