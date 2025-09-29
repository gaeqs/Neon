//
// Created by grial on 15/11/22.
//

#ifndef NEON_SHADERPROGRAM_H
#define NEON_SHADERPROGRAM_H

#include <unordered_map>
#include <optional>
#include <memory>
#include <string>

#include <cmrc/cmrc.hpp>

#include <neon/structure/Asset.h>
#include <neon/render/shader/ShaderType.h>
#include <neon/render/shader/ShaderUniform.h>

#include <neon/util/Result.h>

#ifdef USE_VULKAN

    #include <vulkan/render/shader/VKShaderProgram.h>

#endif

namespace neon
{
    class Room;

    /**
     * Represents a set of shaders that can be used in a material.
     * <p>
     * Shader programs contains all shaders used by a pipeline
     * to render an object.
     * To create a shader program, you must provide a set of shaders
     * in GLSL format.
     * When all shaders are added to the program, you must compile
     * it using the method <i>compile</i>.
     * You cannot use a shader program that is not compiled.
     * <p>
     * You may use the util static method <i>createShader</i> to
     * create an compile a shader program in one line.
     */
    class ShaderProgram : public Asset
    {
        template<class T>
        friend class IdentifiableWrapper;

      public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKShaderProgram;
#endif

      private:
        bool _compiled;
        std::unordered_map<ShaderType, std::string> _rawShaders;
        Implementation _implementation;

      public:
        ShaderProgram(const ShaderProgram& other) = delete;

        /**
         * Creates a new shader program.
         * <p>
         * You may add shaders to this program using the method
         * <i>addShader</i>.
         * @param application the application holding this asset.
         * @param name the name of this shader program.
         */
        ShaderProgram(Application* application, std::string name);

        /**
         * Returns the implementation of this shader program.
         * @return the implementation.
         */
        [[nodiscard]] const Implementation& getImplementation() const;

        /**
         * Returns the implementation of this shader program.
         * @return the implementation.
         */
        Implementation& getImplementation();

        /**
         * Adds a shader to this program.
         * You can only add new shaders when the program is not
         * compiled.
         * Once this shader program is compiled, no new shaders
         * can be added, and this method will always return false.
         * @param type the type of the shader.
         * @param resource the shader data.
         * @return whether the shader was added.
         */
        bool addShader(ShaderType type, cmrc::file resource);

        /**
         * Adds a shader to this program.
         * You can only add new shaders when the program is not
         * compiled.
         * Once this shader program is compiled, no new shaders
         * can be added, and this method will always return false.
         * @param type the type of the shader.
         * @param resource the shader data.
         * @return whether the shader was added.
         */
        bool addShader(ShaderType type, std::string resource);

        /**
         * @brief Compiles this shader program.
         *
         * @warning No new shaders can be added after the program has been compiled.
         *
         * @return a compilation error if present.
         */
        std::optional<std::string> compile();

        /**
         * @brief Compiles the shader program, processing #include directives.
         *
         * This overload handles file inclusion (#include) during shader
         * preprocessing, similar to C/C++.
         *
         * - `#include "path/to/shader.glsl"`: Resolved relative to `includerRootPath`.
         * - `#include <library_shader.glsl>`: Resolved relative to the root of 'includeFileSystem'.
         *
         * @warning No new shaders can be added after the program has been compiled.
         *
         * @param includerFileSystem The file system for system includes (`<...>`).
         * @param includerRootPath The base directory for local includes (`"..._).
         * @return A std::string containing the compilation error if one occurs, or nothing on success.
         */
        std::optional<std::string> compile(FileSystem* includerFileSystem, std::filesystem::path includerRootPath);

        const std::vector<ShaderUniformBlock>& getUniformBlocks() const;

        const std::vector<ShaderUniformSampler>& getUniformSamplers() const;

        // region Util static methods

        /**
         * Util static method that creates a new shader program
         * that only has a vertex and fragment shader.
         * The returned program is already compiled;
         * you cannot add new shaders to the returned program.
         * @param app the application holding the program.
         * @param name the name of the program.
         * @param vert the vertex shader on GLSL source format.
         * @param frag the fragment shader on GLSL source format.
         * @return the program or a compilation error.
         */
        static neon::Result<std::shared_ptr<ShaderProgram>, std::string>

        createShader(Application* app, std::string name, std::string vert, std::string frag);

        /**
         * Util static method that creates a new shader program
         * that only has a vertex and fragment shader.
         * The returned program is already compiled;
         * you cannot add new shaders to the returned program.
         * @param app the application holding the program.
         * @param name the name of the program.
         * @param vert the vertex shader on GLSL source format.
         * @param frag the fragment shader on GLSL source format.
         * @return the program or a compilation error.
         */
        static neon::Result<std::shared_ptr<ShaderProgram>, std::string> createShader(Application* app,
                                                                                      std::string name, cmrc::file vert,
                                                                                      cmrc::file frag);

        // endregion
    };
} // namespace neon

#endif // NEON_SHADERPROGRAM_H
