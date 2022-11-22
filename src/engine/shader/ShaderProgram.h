//
// Created by grial on 15/11/22.
//

#ifndef NEON_SHADERPROGRAM_H
#define NEON_SHADERPROGRAM_H

#include <unordered_map>
#include <optional>

#include <cmrc/cmrc.hpp>

#include <engine/Identifiable.h>
#include <engine/shader/ShaderType.h>

#ifdef USE_OPENGL

#include <gl/GLShaderProgram.h>

#endif
#ifdef USE_VULKAN

#include <vulkan/shader/VKShaderProgram.h>

#endif

class Room;

class ShaderProgram : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

public:
#ifdef USE_OPENGL
    using Implementation = GLShaderProgram;
#endif
#ifdef USE_VULKAN
    using Implementation = VKShaderProgram;
#endif

private:
    uint64_t _id;
    bool _compiled;
    std::unordered_map<ShaderType, cmrc::file> _rawShaders;
    Implementation _implementation;

public:


    ShaderProgram(const ShaderProgram& other) = delete;

    explicit ShaderProgram(Room* room);

    [[nodiscard]] uint64_t getId() const override;

    [[nodiscard]] const Implementation& getImplementation() const;

    Implementation& getImplementation();

    bool addShader(ShaderType type, cmrc::file resource);

    /*bool defineBinding(uint32_t binding, uint32_t size, bool instanceMode);

    bool defineAttribute(
            uint32_t location,
            uint32_t binding,
            uint32_t sizeInFloats,
            uint32_t bindingOffset);*/

    std::optional<std::string> compile();

};


#endif //NEON_SHADERPROGRAM_H
