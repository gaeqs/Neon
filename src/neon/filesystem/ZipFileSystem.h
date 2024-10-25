//
// Created by gaeqs on 25/10/24.
//

#ifndef ZIPFILESYSTEM_H
#define ZIPFILESYSTEM_H

#include <neon/filesystem/FileSystem.h>

#include <zip.h>

namespace neon {
    class ZipFileSystem : public FileSystem {
        zip_t* _zip;

    public:
        ZipFileSystem(std::filesystem::path file);

        ~ZipFileSystem() override;

        std::unique_ptr<File> readFile(std::filesystem::path path) override;

        bool exists(std::filesystem::path path) override;
    };
}


#endif //ZIPFILESYSTEM_H
