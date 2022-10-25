//
// Created by grial on 19/10/22.
//

#include "Shader.h"

#include <glad/glad.h>
#include <iostream>

constexpr int LOG_SIZE = 1024;

Result<std::shared_ptr<Shader>, std::string>
Shader::newShader(const cmrc::file& vertex, const cmrc::file& fragment) {
    char log[LOG_SIZE];
    int32_t success;
    const GLchar* vertexData = vertex.begin();
    const GLchar* fragmentData = fragment.begin();

    std::cout << vertexData << std::endl;
    std::cout << fragmentData << std::endl;

    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexData, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, LOG_SIZE, nullptr, log);
        return {"Error compiling vertex shader: " + std::string(log)};
    }

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentData, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, LOG_SIZE, nullptr, log);
        return {"Error compiling fragment shader: " + std::string(log)};
    }

    uint32_t id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    glGetProgramiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, LOG_SIZE, nullptr, log);
        return {"Error linking shader program: " + std::string(log)};
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return {std::make_shared<Shader>(id)};
}

int32_t Shader::fetchUniformLocation(const std::string& name) {
    auto it = _uniformLocations.find(name);
    if (it != _uniformLocations.end()) {
        return it->second;
    } else {
        int32_t location = glGetUniformLocation(_id, name.c_str());
        _uniformLocations[name] = location;
        return location;
    }
}

Shader::Shader(uint32_t id) : _id(id), _uniformLocations() {
}

Shader::~Shader() {
    glDeleteProgram(_id);
}

void Shader::use() const {
    glUseProgram(_id);
}

void Shader::setUniform(const std::string& name, bool value) {
    glUniform1i(fetchUniformLocation(name), static_cast<int>(value));
}

void Shader::setUniform(const std::string& name, int value) {
    glUniform1i(fetchUniformLocation(name), value);
}

void Shader::setUniform(const std::string& name, unsigned int value) {
    glUniform1ui(fetchUniformLocation(name), value);
}

void Shader::setUniform(const std::string& name, float value) {
    glUniform1f(fetchUniformLocation(name), value);
}

void Shader::setUniform(const std::string& name, const glm::vec2& value) {
    glUniform2fv(fetchUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) {
    glUniform3fv(fetchUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, const glm::vec4& value) {
    glUniform4fv(fetchUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(fetchUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void
Shader::setUniform(const std::string& name, int32_t size, const float* array) {
    glUniform1fv(fetchUniformLocation(name), size, array);
}
