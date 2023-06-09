//
// Created by grial on 15/12/22.
//

#ifndef NEON_VKRENDERPASS_H
#define NEON_VKRENDERPASS_H

#include <vector>
#include <vulkan/vulkan.h>

#include <engine/render/FrameBuffer.h>

namespace neon {
    class Application;
}

namespace neon::vulkan {

    class AbstractVKApplication;

    class VKRenderPass {

        AbstractVKApplication* _vkApplication;
        VkRenderPass _raw;

    public:

        VKRenderPass(const VKRenderPass& other) = delete;

        VKRenderPass(VKRenderPass&& other) noexcept;

        VKRenderPass(Application* application,
                     const std::vector<VkFormat>& colorFormats,
                     bool depth,
                     bool present,
                     VkFormat depthFormat);

        ~VKRenderPass();

        [[nodiscard]] VkRenderPass getRaw() const;

    };
}

#endif //NEON_VKRENDERPASS_H
