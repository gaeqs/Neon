//
// Created by gaeqs on 24/10/24.
//

#ifndef DIRECTORYFILESYSTEM_H
#define DIRECTORYFILESYSTEM_H

#include <neon/filesystem/FileSystem.h>


namespace neon {
    class DirectoryFileSystem : public FileSystem {
        std::filesystem::path _root;

    public:
        explicit DirectoryFileSystem(std::filesystem::path root);

        ~DirectoryFileSystem() override = default;

        std::optional<File> readFile(std::filesystem::path path) const override;

        bool exists(std::filesystem::path path) const override;
    };
}


#endif //DIRECTORYFILESYSTEM_H
