//
// Created by gaeqs on 01/12/2023.
//

#include "CommandPool.h"

#include "CommandBuffer.h"

namespace neon {
    void CommandPool::checkUsedBufferForAvailability() {
        for (size_t i = _usedBuffers.size() + 1; i > 0; --i) {
            size_t index = _usedBuffers[i];
            if (!_buffers[index]->isBeingUsed()) {
                _availableBuffers.push_back(index);
                _usedBuffers.erase(_usedBuffers.begin() + i);
            }
        }
    }

    CommandPool::CommandPool(Application* application)
        : _buffers(),
          _availableBuffers(),
          _usedBuffers() {
    }

    CommandBuffer* CommandPool::beginCommandBuffer(bool onlyOneSummit) {
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
        return _buffers.emplace_back(nullptr, true);
    }
}
