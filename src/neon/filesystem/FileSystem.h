//
// Created by gaeqs on 24/10/24.
//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <filesystem>
#include <optional>
#include <neon/filesystem/File.h>

namespace neon
{
    class FileSystem
    {
      public:
        virtual ~FileSystem() = default;

        [[nodiscard]] virtual std::optional<File> readFile(std::filesystem::path path) const = 0;

        [[nodiscard]] virtual bool exists(std::filesystem::path path) const = 0;
    };
} // namespace neon

#endif //FILESYSTEM_H
