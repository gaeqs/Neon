//
// Created by gaelr on 4/06/23.
//

#include "VKCommandBuffer.h"

#include <ranges>

#include <neon/Application.h>

#include <vulkan/AbstractVKApplication.h>
#include <neon/render/buffer/CommandPool.h>

namespace neon::vulkan {
    VKCommandBuffer::VKCommandBuffer(VKCommandBuffer&& move) noexcept
        : _vkApplication(move._vkApplication),
          _pool(move._pool),
          _queue(move._queue),
          _commandBuffer(move.getCommandBuffer()),
          _status(move._status),
          _fences(std::move(move._fences)),
          _freedFences(std::move(move._freedFences)),
          _external(move._external) {
        move._pool = VK_NULL_HANDLE;
        move._queue = VK_NULL_HANDLE;
        move._commandBuffer = VK_NULL_HANDLE;
    }

    VKCommandBuffer::VKCommandBuffer(Application* application, bool primary)
        : _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _commandBuffer(VK_NULL_HANDLE),
          _status(VKCommandBufferStatus::READY),
          _fences(),
          _freedFences(),
          _external(false) {
        auto& pool = _vkApplication->getCommandPool()->getImplementation();
        _pool = pool.raw();
        _queue = pool.getQueue();

        if (_queue == nullptr) {
            std::cerr << "Queue is null!" << std::endl;
        }

        VkCommandBufferAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = _pool;
        info.level = primary
                         ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                         : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        info.commandBufferCount = 1;
        if (vkAllocateCommandBuffers(_vkApplication->getDevice()->getRaw(),
                                     &info, &_commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    VKCommandBuffer::VKCommandBuffer(Application* application,
                                     VkCommandPool pool,
                                     VkQueue queue,
                                     bool primary)
        : _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _pool(pool),
          _queue(queue),
          _commandBuffer(VK_NULL_HANDLE),
          _status(VKCommandBufferStatus::READY),
          _fences(),
          _freedFences(),
          _external(false) {
        if (_queue == nullptr) {
            std::cerr << "Queue is null!" << std::endl;
        }

        VkCommandBufferAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = pool;
        info.level = primary
                         ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                         : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        info.commandBufferCount = 1;
        if (vkAllocateCommandBuffers(_vkApplication->getDevice()->getRaw(),
                                     &info, &_commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    VKCommandBuffer::VKCommandBuffer(Application* application,
                                     VkCommandBuffer commandBuffer,
                                     VkQueue queue)
        : _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _commandBuffer(commandBuffer),
          _pool(VK_NULL_HANDLE),
          _queue(queue),
          _status(VKCommandBufferStatus::READY),
          _external(true) {
        if (_queue == nullptr) {
            std::cerr << "Queue is null!" << std::endl;
        }
    }

    VKCommandBuffer::~VKCommandBuffer() {
        free();
    }

    VkCommandBuffer VKCommandBuffer::getCommandBuffer() const {
        return _commandBuffer;
    }

    bool VKCommandBuffer::begin(bool onlyOneSubmit) {
        refreshStatus();
        if (_status != VKCommandBufferStatus::READY) {
            printInvalidState(VKCommandBufferStatus::READY);
            return false;
        }
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = onlyOneSubmit
                              ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
                              : 0;
        VkResult result = vkBeginCommandBuffer(_commandBuffer, &beginInfo);
        if (result != VK_SUCCESS) {
            std::cout << "Error starting command buffer " << _commandBuffer <<
                    ". Resturn status: " << result << "." << std::endl;
            return false;
        }
        _status = VKCommandBufferStatus::RECORDING;
        return true;
    }

    bool VKCommandBuffer::end() {
        if (_status != VKCommandBufferStatus::RECORDING) {
            printInvalidState(VKCommandBufferStatus::RECORDING);
            return false;
        }
        vkEndCommandBuffer(_commandBuffer);
        _status = VKCommandBufferStatus::RECORDED;
        return true;
    }

    bool VKCommandBuffer::submit(uint32_t waitSemaphoreAmount,
                                 VkSemaphore* waitSemaphores,
                                 VkPipelineStageFlags* waitStages,
                                 uint32_t signalSemaphoreAmount,
                                 VkSemaphore* signalSemaphores) {
        if (_status != VKCommandBufferStatus::RECORDED) {
            printInvalidState(VKCommandBufferStatus::RECORDED);
            return false;
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &_commandBuffer;
        submitInfo.waitSemaphoreCount = waitSemaphoreAmount;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.signalSemaphoreCount = signalSemaphoreAmount;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VkQueue queue = _queue;
        VkFence fence = fetchAvailableFence();
        vkQueueSubmit(queue, 1, &submitInfo, fence);
        _fences.push_back(fence);

        return true;
    }

    void VKCommandBuffer::reset(bool releaseResources) {
        waitForFences();

        VkCommandBufferResetFlags bit =
                releaseResources
                    ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
                    : 0;

        vkResetCommandBuffer(_commandBuffer, bit);
        _status = VKCommandBufferStatus::READY;
    }

    void VKCommandBuffer::waitForFences() {
        if (_fences.empty()) return;
        vkWaitForFences(_vkApplication->getDevice()->getRaw(),
                        _fences.size(), _fences.data(), VK_TRUE, UINT64_MAX);

        vkResetFences(_vkApplication->getDevice()->getRaw(),
                      _fences.size(), _fences.data());

        for (const auto& fence: _fences) {
            _freedFences.push_back(fence);
        }

        _fences.clear();
    }

    void VKCommandBuffer::refreshStatus() {
        if (_status == VKCommandBufferStatus::RECORDED) {
            if (!isBeingUsed()) {
                _status = VKCommandBufferStatus::READY;
            }
        }
    }

    VkFence VKCommandBuffer::fetchAvailableFence() {
        if (_freedFences.empty()) {
            return createFence();
        }

        VkFence fence = _freedFences.back();
        _freedFences.pop_back();
        return fence;
    }

    VkFence VKCommandBuffer::createFence() const {
        VkFence fence;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = 0;

        if (vkCreateFence(_vkApplication->getDevice()->getRaw(), &fenceInfo,
                          nullptr,
                          &fence)
            != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fence!");
        }

        return fence;
    }

    void VKCommandBuffer::printInvalidState(
        VKCommandBufferStatus expected) const {
        auto getName = [](VKCommandBufferStatus s) {
            switch (s) {
                case VKCommandBufferStatus::READY:
                    return "READY";
                case VKCommandBufferStatus::RECORDING:
                    return "RECORDING";
                case VKCommandBufferStatus::RECORDED:
                    return "RECORDED";
                default:
                    return "INVALID STATE";
            }
        };

        std::cerr << std::format(
            "Invalid command buffer status. Actual: {}. Expected: {}.",
            getName(_status),
            getName(expected)
        ) << std::endl;
    }

    void VKCommandBuffer::free() {
        if (_commandBuffer == VK_NULL_HANDLE) return;

        waitForFences();

        for (const auto& fence: _freedFences) {
            vkDestroyFence(_vkApplication->getDevice()->getRaw(), fence,
                           nullptr);
        }

        if (!_external) {
            vkFreeCommandBuffers(
                _vkApplication->getDevice()->getRaw(),
                _pool,
                1, &_commandBuffer
            );
        }
    }

    bool VKCommandBuffer::isBeingUsed() const {
        if (_status == VKCommandBufferStatus::RECORDING) return true;
        if (_fences.empty()) return false;
        auto device = _vkApplication->getDevice()->getRaw();

        bool used = std::ranges::any_of(
            _fences,
            [device](const VkFence& it) {
                return vkGetFenceStatus(device, it) == VK_NOT_READY;
            }
        );

        if (!used) {
            vkResetFences(_vkApplication->getDevice()->getRaw(),
                          _fences.size(), _fences.data());
            _freedFences.insert(
                _freedFences.end(),
                _fences.begin(),
                _fences.end()
            );
            _fences.clear();
        }

        return used;
    }

    VKCommandBuffer& VKCommandBuffer::operator
    =(VKCommandBuffer&& move) noexcept {
        if (this == &move) return *this; // SAME!
        free();
        _vkApplication = move._vkApplication;
        _pool = move._pool;
        _commandBuffer = move._commandBuffer;
        _status = move._status;
        _fences = std::move(move._fences);
        _freedFences = std::move(move._freedFences);
        move._pool = VK_NULL_HANDLE;
        move._commandBuffer = VK_NULL_HANDLE;
        return *this;
    }
}
