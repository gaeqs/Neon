//
// Created by grial on 15/11/22.
//

#include "ShaderProgram.h"

#include <utility>

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
        _rawShaders[type] = std::string(resource.begin(), resource.size());
        return true;
    }

    bool ShaderProgram::addShader(ShaderType type, std::string resource) {
        _rawShaders[type] = std::move(resource);
        return true;
    }

    std::optional<std::string> ShaderProgram::compile() {
        if (_compiled) return "Shader already compiled.";
        return _implementation.compile(_rawShaders);
    }

    Result<std::shared_ptr<ShaderProgram>, std::string>
    ShaderProgram::createShader(Application* app, std::string name,
                                std::string vert, std::string frag) {
        auto shader = std::make_shared<ShaderProgram>(app, std::move(name));
        shader->addShader(ShaderType::VERTEX, std::move(vert));
        shader->addShader(ShaderType::FRAGMENT, std::move(frag));

        auto result = shader->compile();
        if (result.has_value()) {
            return result.value();
        }

        return shader;
    }

    neon::Result<std::shared_ptr<ShaderProgram>, std::string>
    ShaderProgram::createShader(Application* app, std::string name,
                                cmrc::file vert, cmrc::file frag) {
        return createShader(app, std::move(name),
                            std::string(vert.begin(), vert.size()),
                            std::string(frag.begin(), frag.size()));
    }
}