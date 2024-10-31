//
// Created by gaeqs on 25/10/2024.
//

#include "ShaderProgramLoader.h"

#include <neon/filesystem/FileSystem.h>
#include <neon/structure/Application.h>

namespace neon {
    std::shared_ptr<ShaderProgram> ShaderProgramLoader::loadAsset(std::string name,
                                                                  nlohmann::json json,
                                                                  AssetLoaderContext context) {
        constexpr std::array TYPES = {
            ShaderType::VERTEX, ShaderType::FRAGMENT, ShaderType::GEOMETRY, ShaderType::TASK, ShaderType::MESH
        };
        constexpr std::array NAMES = {"vertex", "fragment", "geometry", "task", "mesh"};

        std::shared_ptr<ShaderProgram> shader = std::make_shared<ShaderProgram>(context.application, name);

        for (size_t i = 0; i < TYPES.size(); i++) {
            const std::string& typeName = NAMES[i];
            auto object = json[typeName];
            if (!object.is_object()) continue;
            auto file = object["file"];
            if (file.is_string() && context.fileSystem != nullptr) {
                // Load text from file.
                auto relative = std::filesystem::path(file.get<std::string>());
                auto absolute = context.path.has_value() ? context.path.value() / relative : relative;
                auto optional = context.fileSystem->readFile(absolute);
                if (optional.has_value()) {
                    auto text = optional.value().toString();
                    shader->addShader(TYPES[i], text);
                    continue;
                }
            }

            auto raw = object["raw"];
            if (raw.is_string()) {
                shader->addShader(TYPES[i], raw.get<std::string>());
            }
        }

        if (auto result = shader->compile(); result.has_value()) {
            context.application->getLogger().error(result.value());
            return nullptr;
        }

        return shader;
    }
}
