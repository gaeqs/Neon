//
// Created by gaelr on 4/06/23.
//

#include "CommandBuffer.h"

namespace neon {
    CommandBuffer::CommandBuffer(CommandBuffer&& move) noexcept
        : _implementation(std::move(move._implementation)) {
    }

    CommandBuffer::CommandBuffer(Application* application,
                                 bool primary)
        : _implementation(application, primary) {
    }


#ifdef USE_VULKAN

    CommandBuffer::CommandBuffer(Application* application, VkCommandPool pool,
                                 VkQueue queue,
                                 bool primary)
        : _implementation(application, pool, queue, primary) {
    }

    CommandBuffer::CommandBuffer(Application* application,
                                 VkCommandBuffer commandBuffer,
                                 VkQueue queue)
        : _implementation(application, commandBuffer, queue) {
    }

#endif

    const CommandBuffer::Implementation&
    CommandBuffer::getImplementation() const {
        return _implementation;
    }

    CommandBuffer::Implementation& CommandBuffer::getImplementation() {
        return _implementation;
    }

    bool CommandBuffer::begin(bool onlyOneSubmit) {
        return _implementation.begin(onlyOneSubmit);
    }

    bool CommandBuffer::end() {
        return _implementation.end();
    }

    bool CommandBuffer::submit() {
        return _implementation.submit();
    }

    void CommandBuffer::wait() {
        _implementation.waitForFences();
    }

    void CommandBuffer::reset(bool releaseResources) {
        return _implementation.reset(releaseResources);
    }

    bool CommandBuffer::isBeingUsed() const {
        return _implementation.isBeingUsed();
    }

    CommandBuffer& CommandBuffer::operator=(CommandBuffer&& move) noexcept {
        _implementation = std::move(move._implementation);
        return *this;
    }
}
