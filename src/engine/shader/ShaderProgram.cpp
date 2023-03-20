//
// Created by grial on 15/11/22.
//

#include "ShaderProgram.h"

#include <engine/structure/Room.h>

namespace neon {

    ShaderProgram::ShaderProgram(Application* application, std::string name) :
            Asset(typeid(ShaderProgram), std::move(name)),
            _compiled(false),
            _rawShaders(),
            _implementation(application) {

    }

    const ShaderProgram::Implementation&
    ShaderProgram::getImplementation() const {
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
}