//
// Created by grial on 18/11/22.
//

#ifndef NEON_SPIRVCOMPILER_H
#define NEON_SPIRVCOMPILER_H

#include <vector>
#include <string>
#include <optional>

#include <vulkan/vulkan.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include <util/Result.h>

class SPIRVCompiler {

    static TBuiltInResource generateDefaultResources();

    static EShLanguage getLanguage(const VkShaderStageFlagBits& shaderType);

    bool _compiled;
    std::vector<glslang::TShader*> _shaders;
    glslang::TProgram _program;
    TBuiltInResource _resources;

public:

    SPIRVCompiler();

    ~SPIRVCompiler();

    std::optional<std::string>
    addShader(const VkShaderStageFlagBits& shaderType, const char* source);

    std::optional<std::string> compile();

    Result<std::vector<uint32_t>, std::string>
    getStage(const VkShaderStageFlagBits& shaderType);
};


#endif //NEON_SPIRVCOMPILER_H
