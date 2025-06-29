//
// Created by grial on 22/11/22.
//

#ifndef NEON_VKMATERIAL_H
#define NEON_VKMATERIAL_H

#include <vector>
#include <string>

#include <vulkan/vulkan.h>

#include <neon/render/shader/MaterialCreateInfo.h>

namespace neon
{
    class Application;

    class Material;

    class FrameBuffer;
} // namespace neon

namespace neon::vulkan
{
    class AbstractVKApplication;

    class VKMaterial : public VKResource
    {
        Material* _material;

        VkPipelineLayout _pipelineLayout;
        VkPipeline _pipeline;

        std::vector<char> _pushConstants;
        VkShaderStageFlags _pushConstantStages;

        VkRenderPass _target;

      public:
        VKMaterial(const VKMaterial& other) = delete;

        VKMaterial(Application* application, Material* material, const MaterialCreateInfo& createInfo);

        ~VKMaterial();

        [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

        [[nodiscard]] VkPipeline getPipeline() const;

        [[nodiscard]] VkRenderPass getTarget() const;

        void pushConstant(const std::string& name, const void* data, uint32_t size);

        void uploadConstants(VkCommandBuffer buffer) const;

        void setTexture(const std::string& name, std::shared_ptr<SampledTexture> texture);

        void useMaterial(std::shared_ptr<CommandBufferRun> run);
    };
} // namespace neon::vulkan

#endif //NEON_VKMATERIAL_H
