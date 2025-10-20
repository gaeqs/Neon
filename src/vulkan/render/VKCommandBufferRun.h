//
// Created by gaeqs on 16/04/2025.
//

#ifndef VKCOMMANDBUFFERRUN_H
#define VKCOMMANDBUFFERRUN_H

#include <neon/render/buffer/CommandBufferRun.h>

namespace neon::vulkan
{

    class VKCommandBuffer;

    class VKCommandBufferRun : public CommandBufferRun
    {
        VKCommandBuffer* _cmd;

      public:
        explicit VKCommandBufferRun(VKCommandBuffer* cmd);

        bool hasFinished() override;

        void wait() override;

        void markCompletion();
    };

} // namespace neon::vulkan

#endif // VKCOMMANDBUFFERRUN_H
