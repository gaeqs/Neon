//
// Created by grial on 19/10/22.
//

#include "GLShaderProgram.h"

#include <glad/glad.h>
#include <iostream>

constexpr int LOG_SIZE = 1024;

uint32_t GLShaderProgram::getGLShaderType(ShaderType type) {
    switch (type) {
        case ShaderType::VERTEX:
            return GL_VERTEX_SHADER;
        case ShaderType::FRAGMENT:
            return GL_FRAGMENT_SHADER;
    }
}

int32_t GLShaderProgram::fetchUniformLocation(const std::string& name) {
    auto it = _uniformLocations.find(name);
    if (it != _uniformLocations.end()) {
        return it->second;
    } else {
        if (_id.has_value()) {
            int32_t location = glGetUniformLocation(_id.value(), name.c_str());
            _uniformLocations[name] = location;
            return location;
        }
        return -1;
    }
}

GLShaderProgram::GLShaderProgram([[maybe_unused]] Application* application) :
        _id(0),
        _uniformLocations() {

}

GLShaderProgram::~GLShaderProgram() {
    if (_id.has_value()) {
        glDeleteProgram(_id.value());
    }
}

std::optional<std::string>
GLShaderProgram::compile(
        const std::unordered_map<ShaderType, cmrc::file>& raw) {
    char log[LOG_SIZE];
    int32_t success;

    uint32_t id = glCreateProgram();

    std::vector<uint32_t> shaders;
    shaders.reserve(raw.size());
    for (const auto& [type, resource]: raw) {
        const GLchar* data = resource.begin();
        auto size = static_cast<GLint>(resource.size());

        uint32_t shader = glCreateShader(getGLShaderType(type));
        glShaderSource(shader, 1, &data, &size);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, LOG_SIZE, nullptr, log);
            return "Error compiling vertex shader: " + std::string(log);
        }
        glAttachShader(id, shader);
        shaders.push_back(shader);
    }

    glLinkProgram(id);
    glGetProgramiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, LOG_SIZE, nullptr, log);
        return "Error linking shader program: " + std::string(log);
    }

    for (const auto& shader: shaders) {
        glDeleteShader(shader);
    }

    _id = id;
    return {};
}

void GLShaderProgram::use() const {
    if (_id.has_value()) {
        glUseProgram(_id.value());
    }
}

void GLShaderProgram::setUniform(const std::string& name, bool value) {
    glUniform1i(fetchUniformLocation(name), static_cast<int>(value));
}

void GLShaderProgram::setUniform(const std::string& name, int value) {
    glUniform1i(fetchUniformLocation(name), value);
}

void GLShaderProgram::setUniform(const std::string& name, unsigned int value) {
    glUniform1ui(fetchUniformLocation(name), value);
}

void GLShaderProgram::setUniform(const std::string& name, float value) {
    glUniform1f(fetchUniformLocation(name), value);
}

void
GLShaderProgram::setUniform(const std::string& name, const glm::vec2& value) {
    glUniform2fv(fetchUniformLocation(name), 1, &value[0]);
}

void
GLShaderProgram::setUniform(const std::string& name, const glm::vec3& value) {
    glUniform3fv(fetchUniformLocation(name), 1, &value[0]);
}

void
GLShaderProgram::setUniform(const std::string& name, const glm::vec4& value) {
    glUniform4fv(fetchUniformLocation(name), 1, &value[0]);
}

void
GLShaderProgram::setUniform(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(fetchUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void
GLShaderProgram::setUniform(const std::string& name, int32_t size,
                            const float* array) {
    glUniform1fv(fetchUniformLocation(name), size, array);
}
