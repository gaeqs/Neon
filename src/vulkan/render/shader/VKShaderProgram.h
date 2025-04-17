//
// Created by grial on 15/11/22.
//

#ifndef NEON_VKSHADERPROGRAM_H
#define NEON_VKSHADERPROGRAM_H

#include <vector>
#include <optional>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include <neon/render/shader/ShaderType.h>
#include <neon/render/shader/ShaderUniform.h>
#include <vulkan/VKResource.h>

namespace neon
{
    class Application;
}

namespace neon::vulkan
{
    class AbstractVKApplication;

    class VKShaderProgram : public VKResource
    {
        static VkShaderStageFlagBits getStage(ShaderType type);

        AbstractVKApplication* _vkApplication;
        std::vector<VkPipelineShaderStageCreateInfo> _shaders;

        std::vector<ShaderUniformBlock> _uniformBlocks;
        std::vector<ShaderUniformSampler> _samplers;

        void deleteShaders();

      public:
        VKShaderProgram(const VKShaderProgram& other) = delete;

        explicit VKShaderProgram(Application* application);

        ~VKShaderProgram();

        std::optional<std::string> compile(const std::unordered_map<ShaderType, std::string>& raw);

        [[nodiscard]] const std::vector<VkPipelineShaderStageCreateInfo>& getShaders() const;

        [[nodiscard]] const std::vector<ShaderUniformBlock>& getUniformBlocks() const;

        [[nodiscard]] const std::vector<ShaderUniformSampler>& getUniformSamplers() const;
    };
} // namespace neon::vulkan

#endif //NEON_VKSHADERPROGRAM_H
