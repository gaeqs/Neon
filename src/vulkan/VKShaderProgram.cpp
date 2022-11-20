//
// Created by grial on 15/11/22.
//

#include "VKShaderProgram.h"

#include <engine/Application.h>
#include <vulkan/spirv/SPIRVCompiler.h>

VkShaderStageFlagBits VKShaderProgram::getStage(ShaderType type) {
    switch (type) {
        case ShaderType::VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderType::FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        default:
            return VK_SHADER_STAGE_ALL;
    }
}

void VKShaderProgram::deleteShaders() {
    for (const auto& item: _shaders) {
        vkDestroyShaderModule(
                _vkApplication->getDevice(), item.module, nullptr);
    }
    _shaders.clear();
}

VKShaderProgram::VKShaderProgram(Application* application) :
        _vkApplication(&application->getImplementation()),
        _shaders() {
}

VKShaderProgram::~VKShaderProgram() {
    deleteShaders();
}

std::optional<std::string>
VKShaderProgram::compile(
        const std::unordered_map<ShaderType, cmrc::file>& raw) {
    deleteShaders();

    SPIRVCompiler compiler;

    for (const auto& [type, resource]: raw) {
        auto error = compiler.addShader(getStage(type), resource.begin());
        if (error.has_value()) return error;
    }

    auto error = compiler.compile();
    if (error.has_value()) return error;

    for (const auto& [type, _]: raw) {
        auto stage = getStage(type);
        auto result = compiler.getStage(stage).getResult(); // Always OK.

        VkShaderModuleCreateInfo moduleInfo{};
        moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleInfo.codeSize = result.size() * sizeof(uint32_t);
        moduleInfo.pCode = result.data();

        VkShaderModule shaderModule;

        if (vkCreateShaderModule(_vkApplication->getDevice(), &moduleInfo,
                                 nullptr, &shaderModule) != VK_SUCCESS) {
            return "Failed to create shader module.";
        }

        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage = stage;
        stageInfo.module = shaderModule;
        stageInfo.pName = "main";

        _shaders.push_back(stageInfo);
    }

    return {};
}

const std::vector<VkPipelineShaderStageCreateInfo>&
VKShaderProgram::getShaders() const {
    return _shaders;
}

