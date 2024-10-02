//
// Created by gaeqs on 01/10/2024.
//

#ifndef VKDRAWABLE_H
#define VKDRAWABLE_H

#include <vector>
#include <neon/render/shader/Material.h>
#include <neon/render/shader/ShaderUniformBuffer.h>
#include <vulkan/vulkan.h>

namespace neon {
    class Model;
}

namespace neon::vulkan {
    class VKDrawable {
    public:
        virtual ~VKDrawable() = default;

        virtual void draw(
            const Material* material,
            VkCommandBuffer commandBuffer,
            const Model& model,
            const ShaderUniformBuffer* globalBuffer) = 0;
    };
}


#endif //VKDRAWABLE_H
