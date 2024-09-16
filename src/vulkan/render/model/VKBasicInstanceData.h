//
// Created by gaelr on 9/15/2024.
//

#ifndef VKBASICINSTANCEDATA_H
#define VKBASICINSTANCEDATA_H

#include <vector>
#include <vulkan/render/buffer/Buffer.h>
#include <vulkan/render/model/VKInstanceData.h>
#include <neon/util/Range.h>

namespace neon {
    class Application;

    class CommandBuffer;

    struct ModelCreateInfo;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKBasicInstanceData : public VKInstanceData {
        AbstractVKApplication* _vkApplication;
        std::vector<std::unique_ptr<Buffer>> _buffers;

    public:
        VKBasicInstanceData(Application* application,
                            const ModelCreateInfo& info);

        [[nodiscard]] const std::vector<std::unique_ptr<Buffer>>&
        getBuffers() const override;

        void flush(const CommandBuffer* command,
                   size_t index,
                   size_t instanceSize,
                   void* data,
                   Range<uint32_t> changeRange) const;
    };
}

#endif //VKBASICINSTANCEDATA_H
