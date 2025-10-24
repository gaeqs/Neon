//
// Created by grial on 18/11/22.
//

#ifndef NEON_SPIRVCOMPILER_H
#define NEON_SPIRVCOMPILER_H

#include <vector>
#include <string>
#include <optional>

#include <vulkan/vulkan.h>
#include <glslang/Public/ShaderLang.h>
#include <neon/render/shader/ShaderUniform.h>
#include <neon/filesystem/FileSystem.h>
#include <neon/render/shader/IncluderCreateInfo.h>
#include <neon/util/Result.h>
#include <vulkan/device/VKPhysicalDevice.h>

namespace neon::vulkan
{
    class SPIRVIncluder : public glslang::TShader::Includer
    {
        FileSystem* _fileSystem;
        std::filesystem::path _rootPath;

        std::unordered_map<std::filesystem::path, std::string> _cache;

        std::string* fetch(std::filesystem::path path);

      public:
        explicit SPIRVIncluder(IncluderCreateInfo includerCreateInfo);

        ~SPIRVIncluder() override = default;

        IncludeResult* includeSystem(const char*, const char*, size_t) override;

        IncludeResult* includeLocal(const char*, const char*, size_t) override;

        void releaseInclude(IncludeResult*) override;
    };

    class SPIRVCompiler
    {
        static TBuiltInResource generateDefaultResources(const VKPhysicalDevice& device);

        static EShLanguage getLanguage(const VkShaderStageFlagBits& shaderType);

        bool _compiled;
        std::vector<glslang::TShader*> _shaders;
        glslang::TProgram _program;
        TBuiltInResource _resources;
        SPIRVIncluder _includer;

      public:
        SPIRVCompiler(const VKPhysicalDevice& device, IncluderCreateInfo includerCreateInfo);

        ~SPIRVCompiler();

        std::optional<std::string> addShader(const VkShaderStageFlagBits& shaderType, const std::string& source);

        std::optional<std::string> compile();

        Result<std::vector<uint32_t>, std::string> getStage(const VkShaderStageFlagBits& shaderType);

        [[nodiscard]] std::vector<ShaderUniformBlock> getUniformBlocks() const;

        //[[nodiscard]] std::unordered_map<std::string, VKShaderUniform>
        // getBuffers() const;

        [[nodiscard]] std::vector<ShaderUniformSampler> getSamplers() const;
    };
} // namespace neon::vulkan

#endif // NEON_SPIRVCOMPILER_H
