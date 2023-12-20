//
// Created by gaeqs on 01/12/2023.
//

#include "VKCommandPool.h"

#include <vulkan/AbstractVKApplication.h>

neon::vulkan::VKCommandPool::VKCommandPool(
    Application* application)
    : _vkApplication(dynamic_cast<AbstractVKApplication*>(
        application->getImplementation())) {

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = _vkApplication->getGraphicsQueue();

    if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
        }
}
