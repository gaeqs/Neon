//
// Created by gaelr on 9/15/2024.
//

#ifndef VKCONCURRENTINSTANCEDATA_H
#define VKCONCURRENTINSTANCEDATA_H

#include <vector>
#include <vulkan/render/buffer/Buffer.h>
#include <vulkan/render/model/VKInstanceData.h>
#include <neon/util/Range.h>

namespace neon
{
    class Application;

    class CommandBuffer;

    struct ModelCreateInfo;
} // namespace neon

namespace neon::vulkan
{
    class AbstractVKApplication;

    class VKConcurrentInstanceData : public VKInstanceData
    {
        AbstractVKApplication* _vkApplication;
        std::vector<std::unique_ptr<Buffer>> _buffers;

      public:
        VKConcurrentInstanceData(Application* application, const ModelCreateInfo& info);

        [[nodiscard]] const std::vector<std::unique_ptr<Buffer>>& getBuffers() const override;

        void flush(const CommandBuffer* command, size_t index, size_t instanceSize, void* data,
                   Range<uint32_t> changeRange) const;
    };
} // namespace neon::vulkan

#endif //VKCONCURRENTINSTANCEDATA_H
