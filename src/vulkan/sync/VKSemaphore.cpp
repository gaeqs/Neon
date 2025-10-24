//
// Created by gaeqs on 20/10/25.
//

#include "VKSemaphore.h"

#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan
{
    VKSemaphore::VKSemaphore(Application* application) :
        VKResource(application)
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(holdRawDevice(), &semaphoreInfo, nullptr, &_semaphore) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create synchronization objects!");
        }
    }

    VKSemaphore::~VKSemaphore()
    {
        auto bin = getApplication()->getBin();
        auto runs = getRuns();
        bin->destroyLater(getApplication()->getDevice(), runs, _semaphore, vkDestroySemaphore);
    }

    VkSemaphore VKSemaphore::getRaw() const
    {
        return _semaphore;
    }
} // namespace neon::vulkan