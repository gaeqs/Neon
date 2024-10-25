//
// Created by gaeqs on 25/10/24.
//

#ifndef CMRCFILESYSTEM_H
#define CMRCFILESYSTEM_H

#include <cmrc/cmrc.hpp>
#include <neon/filesystem/FileSystem.h>

namespace neon {
    class CMRCFileSystem : public FileSystem {
        cmrc::embedded_filesystem _filesystem;

    public:
        explicit CMRCFileSystem(cmrc::embedded_filesystem filesystem);

        ~CMRCFileSystem() override = default;

        std::unique_ptr<File> readFile(std::filesystem::path path) override;

        bool exists(std::filesystem::path path) override;
    };
}


#endif //CMRCFILESYSTEM_H
