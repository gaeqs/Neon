//
// Created by grial on 22/11/22.
//

#ifndef NEON_VKMATERIAL_H
#define NEON_VKMATERIAL_H

#include <vector>
#include <unordered_map>
#include <string>

#include <vulkan/vulkan.h>

#include <engine/render/Texture.h>
#include <engine/model/InputDescription.h>
#include <engine/shader/MaterialCreateInfo.h>

namespace neon {
    class Application;

    class Material;

    class FrameBuffer;
}

namespace neon::vulkan {
    class VKApplication;

    class VKMaterial {

        Material* _material;

        VKApplication* _vkApplication;
        VkPipelineLayout _pipelineLayout;
        VkPipeline _pipeline;

        std::vector<char> _pushConstants;
        VkShaderStageFlags _pushConstantStages;

        VkRenderPass _target;

    public:

        VKMaterial(const VKMaterial& other) = delete;

        VKMaterial(Application* application,
                   Material* material,
                   const MaterialCreateInfo& createInfo);

        ~VKMaterial();

        [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

        [[nodiscard]] VkPipeline getPipeline() const;

        [[nodiscard]] VkRenderPass getTarget() const;

        void pushConstant(const std::string& name,
                          const void* data,
                          uint32_t size);

        void uploadConstants(VkCommandBuffer buffer) const;

        void setTexture(const std::string& name,
                        std::shared_ptr<Texture> texture);
    };
}

#endif //NEON_VKMATERIAL_H
