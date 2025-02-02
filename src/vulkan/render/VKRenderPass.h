//
// Created by grial on 15/12/22.
//

#ifndef NEON_VKRENDERPASS_H
#define NEON_VKRENDERPASS_H

#include <vector>
#include <vulkan/vulkan.h>

#include <neon/render/buffer/FrameBuffer.h>
#include <neon/render/texture/TextureCreateInfo.h>

namespace neon {
    class Application;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKRenderPass {
        AbstractVKApplication* _vkApplication;
        VkRenderPass _raw;
        bool _external;

    public:
        VKRenderPass(const VKRenderPass& other) = delete;

        VKRenderPass(VKRenderPass&& other) noexcept;

        VKRenderPass(Application* application,
                     const std::vector<VkFormat>& colorFormats,
                     const std::vector<VkSampleCountFlagBits>& samples,
                     bool depth,
                     bool present,
                     bool resolve,
                     VkFormat depthFormat,
                     VkSampleCountFlagBits depthSamples);

        VKRenderPass(Application* application, VkRenderPass pass);

        ~VKRenderPass();

        [[nodiscard]] VkRenderPass getRaw() const;
    };
}

#endif //NEON_VKRENDERPASS_H
