//
// Created by gaeqs on 24/10/24.
//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <memory>
#include <filesystem>
#include <neon/filesystem/File.h>

namespace neon {
    class FileSystem {
    public:
        virtual ~FileSystem() = default;

        virtual std::unique_ptr<File> readFile(std::filesystem::path path) = 0;

        virtual bool exists(std::filesystem::path path) = 0;
    };
}


#endif //FILESYSTEM_H
