//
// Created by grial on 15/11/22.
//

#ifndef NEON_SHADERPROGRAM_H
#define NEON_SHADERPROGRAM_H

#include <unordered_map>
#include <optional>

#include <cmrc/cmrc.hpp>

#include <engine/structure/Asset.h>
#include <engine/shader/ShaderType.h>

#ifdef USE_VULKAN

#include <vulkan/shader/VKShaderProgram.h>

#endif

namespace neon {
    class Room;

    class ShaderProgram : public Asset {

        template<class T> friend
        class IdentifiableWrapper;

    public:
#ifdef USE_VULKAN
    using Implementation = vulkan::VKShaderProgram;
#endif

    private:

        bool _compiled;
        std::unordered_map<ShaderType, cmrc::file> _rawShaders;
        Implementation _implementation;

    public:

        ShaderProgram(const ShaderProgram& other) = delete;

        explicit ShaderProgram(Application* application, std::string name);

        [[nodiscard]] const Implementation& getImplementation() const;

        Implementation& getImplementation();

        bool addShader(ShaderType type, cmrc::file resource);

        std::optional<std::string> compile();

    };
}


#endif //NEON_SHADERPROGRAM_H
