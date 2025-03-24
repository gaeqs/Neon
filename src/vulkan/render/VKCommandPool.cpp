//
// Created by gaeqs on 01/12/2023.
//

#include "VKCommandPool.h"

#include <neon/logging/Logger.h>
#include <vulkan/AbstractVKApplication.h>

#include <neon/render/buffer/CommandBuffer.h>

namespace neon::vulkan
{
    VKCommandPool::VKCommandPool(VKCommandPool&& move) noexcept :
        _application(move._application),
        _vkApplication(move._vkApplication),
        _raw(move._raw),
        _queueFamilyIndex(move._queueFamilyIndex),
        _external(move._external)
    {
        move._raw = VK_NULL_HANDLE;
    }

    VKCommandPool::VKCommandPool(Application* application, VKQueueFamily::Capabilities capabilities) :
        _application(application),
        _vkApplication(dynamic_cast<AbstractVKApplication*>(application->getImplementation())),
        _raw(),
        _external(false)
    {
        auto optional =
            _vkApplication->getDevice()->getQueueProvider()->getFamilies().getCompatibleQueueFamily(capabilities);

        if (!optional.has_value()) {
            std::stringstream ss;
            ss << "No queue family found!" << std::endl;
            ss << " - Graphics: " << capabilities.graphics << std::endl;
            ss << " - Present: " << capabilities.present << std::endl;
            ss << " - Transfer: " << capabilities.transfer << std::endl;
            logger.error(ss.str());
            throw std::runtime_error(ss.str());
        }
        _queueFamilyIndex = optional.value()->getIndex();

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = _queueFamilyIndex;

        if (vkCreateCommandPool(_vkApplication->getDevice()->getRaw(), &poolInfo, nullptr, &_raw) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }
    }

    VKCommandPool::VKCommandPool(Application* application, VkCommandPool external, uint32_t externalQueueFamilyIndex) :
        _application(application),
        _vkApplication(dynamic_cast<AbstractVKApplication*>(application->getImplementation())),
        _raw(external),
        _queueFamilyIndex(externalQueueFamilyIndex),
        _external(true)
    {
    }

    VKCommandPool::~VKCommandPool()
    {
        if (_raw != VK_NULL_HANDLE && !_external) {
            vkDestroyCommandPool(_vkApplication->getDevice()->getRaw(), _raw, nullptr);
        }
    }

    AbstractVKApplication* VKCommandPool::getVkApplication() const
    {
        return _vkApplication;
    }

    uint32_t VKCommandPool::getQueueFamilyIndex() const
    {
        return _queueFamilyIndex;
    }

    VkCommandPool VKCommandPool::raw() const
    {
        return _raw;
    }

    std::unique_ptr<CommandBuffer> VKCommandPool::newCommandBuffer(bool primary) const
    {
        return std::make_unique<CommandBuffer>(*this, primary);
    }

    VKCommandPool& VKCommandPool::operator=(VKCommandPool&& move) noexcept
    {
        if (this == &move) {
            return *this; // SAME!
        }
        if (_raw != VK_NULL_HANDLE && !_external) {
            vkDestroyCommandPool(_vkApplication->getDevice()->getRaw(), _raw, nullptr);
        }
        _application = move._application;
        _vkApplication = move._vkApplication;
        _raw = move._raw;
        move._raw = VK_NULL_HANDLE;
        return *this;
    }
} // namespace neon::vulkan
