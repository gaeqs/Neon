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
        auto* device = getApplication()->getDevice()->getRaw();

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_semaphore) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create synchronization objects!");
        }
    }

    VKSemaphore::~VKSemaphore()
    {
        auto bin = getApplication()->getBin();
        auto device = getApplication()->getDevice()->getRaw();
        auto runs = getRuns();
        bin->destroyLater(device, runs, _semaphore, vkDestroySemaphore);
    }

    VkSemaphore VKSemaphore::getRaw() const
    {
        return _semaphore;
    }
} // namespace neon::vulkan