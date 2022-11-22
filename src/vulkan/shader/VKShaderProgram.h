//
// Created by grial on 15/11/22.
//

#ifndef NEON_VKSHADERPROGRAM_H
#define NEON_VKSHADERPROGRAM_H

#include <vector>
#include <optional>
#include <string>

#include "cmrc/cmrc.hpp"

#include <vulkan/vulkan.h>

#include "engine/shader/ShaderType.h"

class Application;

class VKApplication;

struct VKShaderUniform;

class VKShaderProgram {

    static VkShaderStageFlagBits getStage(ShaderType type);

    VKApplication* _vkApplication;
    std::vector<VkPipelineShaderStageCreateInfo> _shaders;
    std::vector<VKShaderUniform> _uniforms;

    void deleteShaders();

public:

    VKShaderProgram(const VKShaderProgram& other) = delete;

    explicit VKShaderProgram(Application* application);

    ~VKShaderProgram();

    std::optional<std::string>
    compile(const std::unordered_map<ShaderType, cmrc::file>& raw);

    [[nodiscard]] const std::vector<VkPipelineShaderStageCreateInfo>&
    getShaders() const;

    [[nodiscard]] const std::vector<VKShaderUniform>& getUniforms() const;
};


#endif //NEON_VKSHADERPROGRAM_H
