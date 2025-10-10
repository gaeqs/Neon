//
// Created by gaeqs on 10/10/2025.
//

#ifndef NEON_INCLUDERCREATEINFO_H
#define NEON_INCLUDERCREATEINFO_H

#include <string>
#include <unordered_map>
#include <filesystem>

#include <neon/filesystem/FileSystem.h>

namespace neon
{
    /**
     * Provides the necessary information for the shader compiler to
     * resolve include directives within shader source code, similar to how
     * C/C++ preprocessors handle includes.
     */
    struct IncluderCreateInfo
    {
        /**
         * The filesystem that stores the includes.
         */
        FileSystem* fileSystem = nullptr;

        /**
         * The root path used to resolve local includes (#include "...")
         * for the root code snippets (snippets that weren't included).
         */
        std::filesystem::path rootPath;

        /**
         * These are includes that are already prefetched by the includer.
         * This is useful if you want to add extra includes that are not present
         * in the filesystem to the compilation process.
         *
         * These prefetched includes overrides the elements inside the filesystem with the same path.
         */
        std::unordered_map<std::filesystem::path, std::string> prefetchedIncludes;
    };
} // namespace neon

#endif // NEON_INCLUDERCREATEINFO_H
