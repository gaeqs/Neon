//
// Created by gaeqs on 24/10/24.
//

#ifndef DIRECTORYFILESYSTEM_H
#define DIRECTORYFILESYSTEM_H

#include <neon/filesystem/FileSystem.h>

namespace neon
{
    class DirectoryFileSystem : public FileSystem
    {
        std::filesystem::path _root;

      public:
        explicit DirectoryFileSystem(std::filesystem::path root);

        ~DirectoryFileSystem() override = default;

        [[nodiscard]] std::optional<File> readFile(std::filesystem::path path) const override;

        [[nodiscard]] bool exists(std::filesystem::path path) const override;
    };
} // namespace neon

#endif //DIRECTORYFILESYSTEM_H
