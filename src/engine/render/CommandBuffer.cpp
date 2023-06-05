//
// Created by gaelr on 4/06/23.
//

#include "CommandBuffer.h"

namespace neon {

    CommandBuffer::CommandBuffer(Application* application, bool primary) :
            _implementation(application, primary) {
    }

#ifdef USE_VULKAN

    CommandBuffer::CommandBuffer(vulkan::VKApplication* application,
                                 bool primary) :
            _implementation(application, primary) {
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

    void CommandBuffer::reset(bool releaseResources) {
        return _implementation.reset(releaseResources);
    }

}
