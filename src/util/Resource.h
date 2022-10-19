//
// Created by grial on 10/10/22.
//

#ifndef VULKANTEST_RESOURCE_H
#define VULKANTEST_RESOURCE_H


#include <cstdlib>

/**
 * Represents an embedded resource.
 */
class Resource {

    const char* _data;
    size_t _size;

public:

    Resource(const char* start, const char* end);

    const char* data() const;

    size_t size() const;

    const char* begin() const;

    const char* end() const;

};

#define STRING(s) #s

/**
 * Fetches a embedded resource.
 * <p>
 * To load a resource, it must be embedded using the CMake function "add_resources".
 * The resource must be placed inside the "src" folder.
 * <p>
 * You must provide the file's identifier to this macro.
 * The file's identifier is its relative path to "src", replacing the dots and slashes with underscores.
 * Example: "src/foo/bar.txt" becomes "foo_bar_txt".
 * <p>
 * The resource will be placed inside a constant named after the first given parameter.
 */
#define RESOURCE(name, x) \
        extern "C" const char _binary_##x##_start, _binary_##x##_end;   \
        const Resource name(&_binary_##x##_start, &_binary_##x##_end);  \
/**
 * Fetches an embedded shader.
 * <p>
 * This macro is similar to LOAD_RESOURCES, but for shaders
 * compiled using the Cmake function "add_shaders".
 * The shader must be placed in the "src/shader" folder.
 * <p>
 * You must provide the shader's identifier to this macro.
 * The shader's identifier is its relative path to "src/shader", replacing the dots and slashes with underscores.
 * Example: "src/shader/bar.frag" becomes "bar_frag".
 * <p>
 * The resource will be placed inside a constant named after the first given parameter.
 */
#define SHADER_RESOURCE(name, x) RESOURCE(name, shader_##x##)

#endif //VULKANTEST_RESOURCE_H
