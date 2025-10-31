//
// Created by gaeqs on 01/12/2023.
//

#include "CommandPool.h"

#include "CommandBuffer.h"

namespace neon
{
    void CommandPool::checkUsedBufferForAvailability()
    {
        for (size_t i = _usedBuffers.size(); i > 0; --i) {
            size_t index = _usedBuffers[i - 1];
            if (!_buffers[index]->isBeingUsed()) {
                _buffers[index]->reset();
                _availableBuffers.push_back(index);
                _usedBuffers.erase(_usedBuffers.begin() + i - 1);
            }
        }
    }

    CommandPool::CommandPool(CommandPool&& move) noexcept :
        _implementation(std::move(move._implementation))
    {
        _buffers = std::move(move._buffers);
        _availableBuffers = std::move(move._availableBuffers);
        _usedBuffers = std::move(move._usedBuffers);
    }

    CommandPool::CommandPool(Application* application) :
        _implementation(application)
    {
    }

#ifdef USE_VULKAN

    CommandPool::CommandPool(Application* application, VkCommandPool pool, uint32_t queueFamilyIndex) :
        _implementation(application, pool, queueFamilyIndex)
    {
    }

#endif

    CommandPool::~CommandPool()
    {
        waitForAll();
        _buffers.clear();
    }

    Implementation& CommandPool::getImplementation()
    {
        return _implementation;
    }

    const Implementation& CommandPool::getImplementation() const
    {
        return _implementation;
    }

    CommandBuffer* CommandPool::beginCommandBuffer(bool onlyOneSummit)
    {
        if (_availableBuffers.empty()) {
            checkUsedBufferForAvailability();
        }

        if (!_availableBuffers.empty()) {
            size_t index = _availableBuffers.back();
            auto& buffer = _buffers[index];
            buffer->begin(onlyOneSummit);
            _availableBuffers.pop_back();
            _usedBuffers.push_back(index);
            return buffer.get();
        }

        // All buffers used
        std::unique_ptr ptr = _implementation.newCommandBuffer(true);
        _usedBuffers.push_back(_buffers.size());
        _buffers.push_back(std::move(ptr));

        auto raw = _buffers.back().get();
        raw->begin(true);
        return raw;
    }

    void CommandPool::waitForAll()
    {
        checkUsedBufferForAvailability();
        for (auto cmd : _usedBuffers) {
            _buffers[cmd]->wait();
        }
    }

    CommandPool& CommandPool::operator=(CommandPool&& move) noexcept
    {
        _buffers.clear();

        _implementation = std::move(move._implementation);
        _buffers = std::move(move._buffers);
        _availableBuffers = std::move(move._availableBuffers);
        _usedBuffers = std::move(move._usedBuffers);
        return *this;
    }
} // namespace neon
