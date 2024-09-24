//
// Created by gaelr on 9/15/2024.
//

#include "VKBasicInstanceData.h"

#include <neon/structure/Application.h>
#include <neon/render/buffer/CommandBuffer.h>
#include <neon/render/model/ModelCreateInfo.h>
#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan {
    VKBasicInstanceData::VKBasicInstanceData(Application* application,
                                             const ModelCreateInfo& info)
        : _vkApplication(dynamic_cast<AbstractVKApplication*>(
            application->getImplementation())) {
        _buffers.reserve(info.instanceTypes.size());

        for (const auto& size: info.instanceSizes) {
            uint32_t bufferSize = size * info.maximumInstances;
            if (size == 0) _buffers.push_back(nullptr);
            else {
                _buffers.push_back(std::make_unique<StagingBuffer>(
                    _vkApplication,
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    bufferSize
                ));
            }
        }
    }

    const std::vector<std::unique_ptr<Buffer>>&
    VKBasicInstanceData::getBuffers() const {
        return _buffers;
    }

    void VKBasicInstanceData::flush(const CommandBuffer* command,
                                    size_t index,
                                    size_t instanceSize,
                                    void* data,
                                    Range<uint32_t> changeRange) const {
        if (changeRange.size() == 0) return;
        auto& buffer = _buffers[index];
        if (buffer == nullptr) return;

        changeRange *= instanceSize;
        auto map = buffer->map<char>(changeRange, command);
        if (!map.has_value()) return;

        memcpy(
            map.value()->raw(),
            static_cast<char*>(data) + changeRange.getFrom(),
            changeRange.size()
        );
    }
}
