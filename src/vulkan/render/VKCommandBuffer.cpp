//
// Created by gaelr on 4/06/23.
//

#include "VKCommandBuffer.h"

#include <ranges>

#include <engine/Application.h>

#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan {
    VKCommandBuffer::VKCommandBuffer(Application* application, bool primary)
        : _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _commandBuffer(VK_NULL_HANDLE),
          _status(VKCommandBufferStatus::READY),
          _fences(),
          _freedFences(),
          _external(false) {
        VkCommandBufferAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = _vkApplication->getCommandPool();
        info.level = primary
                         ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                         : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        info.commandBufferCount = 1;
        if (vkAllocateCommandBuffers(_vkApplication->getDevice(),
                                     &info, &_commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    VKCommandBuffer::VKCommandBuffer(Application* application,
                                     VkCommandPool pool, bool primary)
        : _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _commandBuffer(VK_NULL_HANDLE),
          _status(VKCommandBufferStatus::READY),
          _fences(),
          _freedFences(),
          _external(false) {
        VkCommandBufferAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = pool;
        info.level = primary
                         ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                         : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        info.commandBufferCount = 1;
        if (vkAllocateCommandBuffers(_vkApplication->getDevice(),
                                     &info, &_commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    VKCommandBuffer::VKCommandBuffer(Application* application,
                                     VkCommandBuffer
                                     commandBuffer) : _vkApplication(
            dynamic_cast<AbstractVKApplication*>(
                application->getImplementation())),
        _commandBuffer(commandBuffer),
        _status(VKCommandBufferStatus::READY),
        _fences(),
        _freedFences(),
        _external(true) {
    }

    VKCommandBuffer::~VKCommandBuffer() {
        waitForFences();

        for (const auto& fence: _freedFences) {
            vkDestroyFence(_vkApplication->getDevice(), fence, nullptr);
        }

        if (!_external) {
            vkFreeCommandBuffers(_vkApplication->getDevice(),
                                 _vkApplication->getCommandPool(),
                                 1, &_commandBuffer);
        }
    }

    VkCommandBuffer VKCommandBuffer::getCommandBuffer() const {
        return _commandBuffer;
    }

    bool VKCommandBuffer::begin(bool onlyOneSubmit) {
        refreshStatus();
        if (_status != VKCommandBufferStatus::READY) return false;
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = onlyOneSubmit
                              ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
                              : 0;
        vkBeginCommandBuffer(_commandBuffer, &beginInfo);
        _status = VKCommandBufferStatus::RECORDING;
        return true;
    }

    bool VKCommandBuffer::end() {
        if (_status != VKCommandBufferStatus::RECORDING) return false;
        vkEndCommandBuffer(_commandBuffer);
        _status = VKCommandBufferStatus::RECORDED;
        return true;
    }

    bool VKCommandBuffer::submit(uint32_t waitSemaphoreAmount,
                                 VkSemaphore* waitSemaphores,
                                 VkPipelineStageFlags* waitStages,
                                 uint32_t signalSemaphoreAmount,
                                 VkSemaphore* signalSemaphores) {
        if (_status != VKCommandBufferStatus::RECORDED) return false;

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &_commandBuffer;
        submitInfo.waitSemaphoreCount = waitSemaphoreAmount;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.signalSemaphoreCount = signalSemaphoreAmount;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VkFence fence = fetchAvailableFence();
        vkQueueSubmit(_vkApplication->getGraphicsQueue(), 1,
                      &submitInfo, fence);
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
        vkWaitForFences(_vkApplication->getDevice(),
                        _fences.size(), _fences.data(), VK_TRUE, UINT64_MAX);

        vkResetFences(_vkApplication->getDevice(),
                      _fences.size(), _fences.data());

        for (const auto& fence: _fences) {
            _freedFences.push_back(fence);
        }

        _fences.clear();
    }

    void VKCommandBuffer::refreshStatus() {
        if (_status == VKCommandBufferStatus::RECORDED) {
            if (isBeingUsed()) {
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

        if (vkCreateFence(_vkApplication->getDevice(), &fenceInfo, nullptr,
                          &fence)
            != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fence!");
        }

        return fence;
    }

    bool VKCommandBuffer::isBeingUsed() {
        if (_status == VKCommandBufferStatus::RECORDING) return false;
        if (_fences.empty()) return false;
        auto device = _vkApplication->getDevice();

        bool used = std::ranges::any_of(
            _fences,
            [device](const VkFence& it) {
                return vkGetFenceStatus(device, it) == VK_NOT_READY;
            }
        );

        if (!used) {
            _freedFences.insert(
                _freedFences.end(),
                _fences.begin(),
                _fences.end()
            );
        }

        return used;
    }
}
