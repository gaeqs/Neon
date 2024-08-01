//
// Created by gaeqs on 01/12/2023.
//

#include "VKCommandPool.h"

#include <vulkan/AbstractVKApplication.h>

#include <engine/render/CommandBuffer.h>

namespace neon::vulkan {
    VKCommandPool::VKCommandPool(VKCommandPool&& move) noexcept
        : _application(move._application),
          _vkApplication(move._vkApplication),
          _raw(move._raw),
          _external(move._external) {
        move._raw = VK_NULL_HANDLE;
    }

    VKCommandPool::VKCommandPool(
        Application* application)
        : _application(application),
          _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _raw(),
          _external(false) {
        auto optional = _vkApplication->getFamilyIndices().graphics;
        if (!optional.has_value()) {
            throw std::runtime_error("No graphic queue found!");
        }

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = optional.value();

        if (vkCreateCommandPool(_vkApplication->getDevice(), &poolInfo,
                                nullptr, &_raw) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }
    }

    VKCommandPool::VKCommandPool(
        Application* application,
        VkCommandPool external)
        : _application(application),
          _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _raw(external),
          _external(true) {
    }

    VKCommandPool::~VKCommandPool() {
        if (_raw != VK_NULL_HANDLE && !_external) {
            vkDestroyCommandPool(
                _vkApplication->getDevice(),
                _raw,
                nullptr
            );
        }
    }

    VkCommandPool VKCommandPool::raw() const {
        return _raw;
    }

    std::unique_ptr<CommandBuffer> VKCommandPool::newCommandBuffer(
        bool primary) const {
        return std::make_unique<CommandBuffer>(_application, _raw, primary);
    }

    VKCommandPool& VKCommandPool::operator=(VKCommandPool&& move) noexcept {
        if (this == &move) return *this; // SAME!
        if (_raw != VK_NULL_HANDLE) {
            vkDestroyCommandPool(
                _vkApplication->getDevice(),
                _raw,
                nullptr
            );
        }
        _application = move._application;
        _vkApplication = move._vkApplication;
        _raw = move._raw;
        move._raw = VK_NULL_HANDLE;
        return *this;
    }
}
