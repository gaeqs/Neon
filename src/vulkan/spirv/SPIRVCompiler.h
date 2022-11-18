//
// Created by grial on 18/11/22.
//

#ifndef NEON_SPIRVCOMPILER_H
#define NEON_SPIRVCOMPILER_H

#include <vector>
#include <string>

#include <vulkan/vulkan.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include <util/Result.h>

class SPIRVCompiler {

    static TBuiltInResource generateDefaultResources();

    static EShLanguage getLanguage(const VkShaderStageFlagBits& shaderType);

public:

    SPIRVCompiler();

    Result<std::vector<uint32_t>, std::string>
    GLSLtoSPV(const VkShaderStageFlagBits& shaderType, const char* source);

};


#endif //NEON_SPIRVCOMPILER_H
