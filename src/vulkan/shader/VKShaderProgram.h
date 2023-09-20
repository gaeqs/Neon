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

#include <engine/shader/ShaderType.h>
#include <vulkan/VKShaderUniform.h>

namespace neon {
    class Application;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKShaderProgram {

        static VkShaderStageFlagBits getStage(ShaderType type);

        AbstractVKApplication* _vkApplication;
        std::vector<VkPipelineShaderStageCreateInfo> _shaders;

        std::unordered_map<std::string, VKShaderUniformBlock> _uniformBlocks;
        std::unordered_map<std::string, VKShaderUniform> _uniforms;
        std::unordered_map<std::string, VKShaderSampler> _samplers;

        void deleteShaders();

    public:

        VKShaderProgram(const VKShaderProgram& other) = delete;

        explicit VKShaderProgram(Application* application);

        ~VKShaderProgram();

        std::optional<std::string>
        compile(const std::unordered_map<ShaderType, std::string>& raw);

        [[nodiscard]] const std::vector<VkPipelineShaderStageCreateInfo>&
        getShaders() const;

        [[nodiscard]] const std::unordered_map<std::string, VKShaderUniform>&
        getUniforms() const;

        [[nodiscard]] const std::unordered_map<std::string, VKShaderUniformBlock>&
        getUniformBlocks() const;

        [[nodiscard]] const std::unordered_map<std::string, VKShaderSampler>&
        getSamplers() const;
    };
}

#endif //NEON_VKSHADERPROGRAM_H
