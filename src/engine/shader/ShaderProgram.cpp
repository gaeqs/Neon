//
// Created by grial on 15/11/22.
//

#include "ShaderProgram.h"

uint64_t SHADER_PROGRAM_ID_GENERATOR = 1;

ShaderProgram::ShaderProgram(Application* application) :
        _id(SHADER_PROGRAM_ID_GENERATOR++),
        _compiled(false),
        _rawShaders(),
        _implementation(application) {

}

uint64_t ShaderProgram::getId() const {
    return _id;
}

const ShaderProgram::Implementation& ShaderProgram::getImplementation() const {
    return _implementation;
}

ShaderProgram::Implementation& ShaderProgram::getImplementation() {
    return _implementation;
}

bool ShaderProgram::addShader(ShaderType type, cmrc::file resource) {
    if (_compiled) return false;
    _rawShaders[type] = resource;
    return true;
}

std::optional<std::string> ShaderProgram::compile() {
    if (_compiled) return "Shader already compiled.";
    return _implementation.compile(_rawShaders);
}
