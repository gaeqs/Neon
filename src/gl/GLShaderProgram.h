//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_SHADER_H
#define RVTRACKING_SHADER_H


#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>

#include <glm/glm.hpp>
#include <cmrc/cmrc.hpp>

#include <engine/shader/ShaderType.h>

class Application;

class GLShaderProgram {

    static uint32_t getGLShaderType(ShaderType type);

    std::optional<uint32_t> _id;
    std::unordered_map<std::string, int32_t> _uniformLocations;


    int32_t fetchUniformLocation(const std::string& name);

public:

    GLShaderProgram(const GLShaderProgram& other) = delete;

    explicit GLShaderProgram(Application* application);

    ~GLShaderProgram();

    std::optional<std::string>
    compile(const std::unordered_map<ShaderType, cmrc::file>& raw);

    void use() const;

    void setUniform(const std::string& name, bool value);

    void setUniform(const std::string& name, int value);

    void setUniform(const std::string& name, unsigned int value);

    void setUniform(const std::string& name, float value);

    void setUniform(const std::string& name, const glm::vec2& value);

    void setUniform(const std::string& name, const glm::vec3& value);

    void setUniform(const std::string& name, const glm::vec4& value);

    void setUniform(const std::string& name, const glm::mat4& value);

    void setUniform(const std::string& name, int32_t size, const float* array);
};


#endif //RVTRACKING_SHADER_H
