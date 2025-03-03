//
// Created by gaeqs on 24/10/24.
//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <filesystem>
#include <optional>
#include <neon/filesystem/File.h>

namespace neon {
    class FileSystem {
    public:
        virtual ~FileSystem() = default;

        virtual std::optional<File> readFile(std::filesystem::path path) const = 0;

        virtual bool exists(std::filesystem::path path) const = 0;
    };
}


#endif //FILESYSTEM_H
