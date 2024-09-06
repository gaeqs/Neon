//
// Created by gaeqs on 01/12/2023.
//

#include "VKCommandPool.h"

#include <vulkan/AbstractVKApplication.h>

#include <neon/render/buffer/CommandBuffer.h>

namespace neon::vulkan {
    VKCommandPool::VKCommandPool(VKCommandPool&& move) noexcept
        : _application(move._application),
          _vkApplication(move._vkApplication),
          _raw(move._raw),
          _queue(std::move(move._queue)),
          _externalQueue(move._externalQueue),
          _external(move._external) {
        move._raw = VK_NULL_HANDLE;
    }

    VKCommandPool::VKCommandPool(
        Application* application,
        VKQueueFamily::Capabilities capabilities)
        : _application(application),
          _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _raw(),
          _external(false) {
        _queue = _vkApplication->getDevice()->getQueueProvider()
                ->fetchCompatibleQueue(capabilities);

        if (!_queue.isValid()) {
            std::stringstream ss;
            ss << "No queue family found!" << std::endl;
            ss << " - Graphics: " << capabilities.graphics << std::endl;
            ss << " - Present: " << capabilities.present << std::endl;
            ss << " - Transfer: " << capabilities.transfer << std::endl;
            ss << "Queue: " << std::endl;
            ss << " - Family: " << _queue.getFamily() << std::endl;
            ss << " - Index: " << _queue.getIndex() << std::endl;
            ss << " - Valid: " << _queue.isValid() << std::endl;
            throw std::runtime_error(ss.str());
        }

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = _queue.getFamily();

        if (vkCreateCommandPool(_vkApplication->getDevice()->getRaw(),
                                &poolInfo,
                                nullptr, &_raw) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }
    }

    VKCommandPool::VKCommandPool(
        Application* application,
        VkCommandPool external,
        VkQueue externalQueue)
        : _application(application),
          _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _raw(external),
          _externalQueue(externalQueue),
          _external(true) {
    }

    VKCommandPool::~VKCommandPool() {
        if (_raw != VK_NULL_HANDLE && !_external) {
            vkDestroyCommandPool(
                _vkApplication->getDevice()->getRaw(),
                _raw,
                nullptr
            );
        }
    }

    VkCommandPool VKCommandPool::raw() const {
        return _raw;
    }

    VkQueue VKCommandPool::getQueue() const {
        return _external ? _externalQueue : _queue.getQueue();
    }

    std::unique_ptr<CommandBuffer> VKCommandPool::newCommandBuffer(
        bool primary) const {
        return std::make_unique<CommandBuffer>(
            _application,
            _raw,
            getQueue(),
            primary
        );
    }

    VKCommandPool& VKCommandPool::operator=(VKCommandPool&& move) noexcept {
        if (this == &move) return *this; // SAME!
        if (_raw != VK_NULL_HANDLE && !_external) {
            vkDestroyCommandPool(
                _vkApplication->getDevice()->getRaw(),
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
