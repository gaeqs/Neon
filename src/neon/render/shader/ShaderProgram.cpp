//
// Created by grial on 15/11/22.
//

#include "ShaderProgram.h"

#include <utility>

namespace neon
{
    ShaderProgram::ShaderProgram(Application* application, std::string name) :
        Asset(typeid(ShaderProgram), std::move(name)),
        _compiled(false),
        _implementation(application)
    {
    }

    const ShaderProgram::Implementation& ShaderProgram::getImplementation() const
    {
        return _implementation;
    }

    ShaderProgram::Implementation& ShaderProgram::getImplementation()
    {
        return _implementation;
    }

    bool ShaderProgram::addShader(ShaderType type, cmrc::file resource)
    {
        if (_compiled) {
            return false;
        }
        _rawShaders[type] = std::string(resource.begin(), resource.size());
        return true;
    }

    bool ShaderProgram::addShader(ShaderType type, std::string resource)
    {
        _rawShaders[type] = std::move(resource);
        return true;
    }

    std::optional<std::string> ShaderProgram::compile()
    {
        if (_compiled) {
            return "Shader already compiled.";
        }
        auto result = _implementation.compile(_rawShaders, IncluderCreateInfo{});
        if (!result.has_value()) {
            _compiled = true;
        }
        return result;
    }

    std::optional<std::string> ShaderProgram::compile(IncluderCreateInfo includerCreateInfo)
    {
        if (_compiled) {
            return "Shader already compiled.";
        }
        auto result = _implementation.compile(_rawShaders, std::move(includerCreateInfo));
        if (!result.has_value()) {
            _compiled = true;
        }
        return result;
    }

    const std::vector<ShaderUniformBlock>& ShaderProgram::getUniformBlocks() const
    {
        return _implementation.getUniformBlocks();
    }

    const std::vector<ShaderUniformSampler>& ShaderProgram::getUniformSamplers() const
    {
        return _implementation.getUniformSamplers();
    }

    Result<std::shared_ptr<ShaderProgram>, std::string> ShaderProgram::createShader(Application* app, std::string name,
                                                                                    std::string vert, std::string frag)
    {
        auto shader = std::make_shared<ShaderProgram>(app, std::move(name));
        shader->addShader(ShaderType::VERTEX, std::move(vert));
        shader->addShader(ShaderType::FRAGMENT, std::move(frag));

        auto result = shader->compile();
        if (result.has_value()) {
            return result.value();
        }

        return shader;
    }

    Result<std::shared_ptr<ShaderProgram>, std::string> ShaderProgram::createShader(Application* app, std::string name,
                                                                                    cmrc::file vert, cmrc::file frag)
    {
        return createShader(app, std::move(name), std::string(vert.begin(), vert.size()),
                            std::string(frag.begin(), frag.size()));
    }
} // namespace neon
