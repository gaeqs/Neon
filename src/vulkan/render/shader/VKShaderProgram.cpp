//
// Created by grial on 15/11/22.
//

#include "VKShaderProgram.h"

#include <neon/structure/Application.h>
#include <vulkan/render/spirv/SPIRVCompiler.h>

#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan
{
    VkShaderStageFlagBits VKShaderProgram::getStage(ShaderType type)
    {
        switch (type) {
            case ShaderType::VERTEX:
                return VK_SHADER_STAGE_VERTEX_BIT;
            case ShaderType::GEOMETRY:
                return VK_SHADER_STAGE_GEOMETRY_BIT;
            case ShaderType::FRAGMENT:
                return VK_SHADER_STAGE_FRAGMENT_BIT;
            case ShaderType::TASK:
                return VK_SHADER_STAGE_TASK_BIT_EXT;
            case ShaderType::MESH:
                return VK_SHADER_STAGE_MESH_BIT_EXT;
            default:
                return VK_SHADER_STAGE_ALL;
        }
    }

    void VKShaderProgram::deleteShaders()
    {
        auto bin = getApplication()->getBin();
        auto device = getApplication()->getDevice();
        auto runs = getRuns();
        for (const auto& item : _shaders) {
            bin->destroyLater(device, runs, item.module, vkDestroyShaderModule);
        }
        _shaders.clear();
    }

    VKShaderProgram::VKShaderProgram(Application* application) :
        VKResource(application)
    {
    }

    VKShaderProgram::~VKShaderProgram()
    {
        deleteShaders();
    }

    std::optional<std::string> VKShaderProgram::compile(const std::unordered_map<ShaderType, std::string>& raw,
                                                        IncluderCreateInfo includerCreateInfo)
    {
        deleteShaders();

        SPIRVCompiler compiler(getApplication()->getPhysicalDevice(), std::move(includerCreateInfo));

        for (const auto& [type, code] : raw) {
            auto error = compiler.addShader(getStage(type), code);
            if (error.has_value()) {
                return "Error compiling shader:\n" + code + "\n" + error.value();
            }
        }

        auto error = compiler.compile();
        if (error.has_value()) {
            return error;
        }

        for (const auto& [type, _] : raw) {
            auto stage = getStage(type);
            auto result = compiler.getStage(stage).getResult(); // Always OK.

            VkShaderModuleCreateInfo moduleInfo{};
            moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            moduleInfo.codeSize = result.size() * sizeof(uint32_t);
            moduleInfo.pCode = result.data();

            VkShaderModule shaderModule;

            auto holder = holdRawDevice();
            if (vkCreateShaderModule(holder, &moduleInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                return "Failed to create shader module.";
            }

            VkPipelineShaderStageCreateInfo stageInfo{};
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.stage = stage;
            stageInfo.module = shaderModule;
            stageInfo.pName = "main";

            _shaders.push_back(stageInfo);
        }

        _uniformBlocks = compiler.getUniformBlocks();
        _samplers = compiler.getSamplers();

        return {};
    }

    const std::vector<VkPipelineShaderStageCreateInfo>& VKShaderProgram::getShaders() const
    {
        return _shaders;
    }

    const std::vector<ShaderUniformBlock>& VKShaderProgram::getUniformBlocks() const
    {
        return _uniformBlocks;
    }

    const std::vector<ShaderUniformSampler>& VKShaderProgram::getUniformSamplers() const
    {
        return _samplers;
    }
} // namespace neon::vulkan
