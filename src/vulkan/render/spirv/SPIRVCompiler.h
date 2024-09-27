//
// Created by grial on 18/11/22.
//

#ifndef NEON_SPIRVCOMPILER_H
#define NEON_SPIRVCOMPILER_H

#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

#include <vulkan/vulkan.h>
#include <SPIRV/GlslangToSpv.h>

#include <neon/util/Result.h>
#include <vulkan/device/VKPhysicalDevice.h>
#include <vulkan/VKShaderUniform.h>

namespace neon::vulkan {
    class SPIRVCompiler {
        static TBuiltInResource generateDefaultResources(
            const VKPhysicalDevice& device);

        static EShLanguage getLanguage(const VkShaderStageFlagBits& shaderType);

        bool _compiled;
        std::vector<glslang::TShader*> _shaders;
        glslang::TProgram _program;
        TBuiltInResource _resources;

    public:
        SPIRVCompiler(const VKPhysicalDevice& device);

        ~SPIRVCompiler();

        std::optional<std::string>
        addShader(const VkShaderStageFlagBits& shaderType,
                  const std::string& source);

        std::optional<std::string> compile();

        Result<std::vector<uint32_t>, std::string>
        getStage(const VkShaderStageFlagBits& shaderType);

        [[nodiscard]] std::unordered_map<std::string, VKShaderUniformBlock>
        getUniformBlocks() const;

        [[nodiscard]] std::unordered_map<std::string, VKShaderUniform>
        getUniforms() const;

        [[nodiscard]] std::unordered_map<std::string, VKShaderSampler>
        getSamplers() const;
    };
}

#endif //NEON_SPIRVCOMPILER_H
