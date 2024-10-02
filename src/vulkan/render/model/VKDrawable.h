//
// Created by gaeqs on 01/10/2024.
//

#ifndef VKDRAWABLE_H
#define VKDRAWABLE_H

#include <vector>
#include <neon/render/shader/Material.h>
#include <neon/render/shader/ShaderUniformBuffer.h>
#include <vulkan/vulkan.h>

namespace neon::vulkan {
    class VKDrawable {
    public:
        virtual ~VKDrawable() = default;

        virtual void draw(
            const Material* material,
            VkCommandBuffer commandBuffer,
            const std::vector<std::unique_ptr<Buffer>>& instancingBuffers,
            uint32_t instancingElements,
            const ShaderUniformBuffer* globalBuffer,
            const ShaderUniformBuffer* modelBuffer) = 0;
    };
}


#endif //VKDRAWABLE_H
