//
// Created by grial on 15/12/22.
//

#ifndef NEON_VKRENDERPASS_H
#define NEON_VKRENDERPASS_H

#include <vulkan/vulkan.h>

class Application;

class VKRenderPass {

    VkRenderPass _raw;

public:

    VKRenderPass(Application* application,
                 uint32_t colorAttachments, bool depth);

    [[nodiscard]] VkRenderPass getRaw() const;

};


#endif //NEON_VKRENDERPASS_H
