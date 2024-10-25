//
// Created by gaeqs on 25/10/24.
//

#include "CMRCFileSystem.h"

#include <pango/pango-utils.h>


namespace neon {
    CMRCFileSystem::CMRCFileSystem(cmrc::embedded_filesystem filesystem)
        : _filesystem(filesystem) {}

    std::unique_ptr<File> CMRCFileSystem::readFile(std::filesystem::path path) {
        if (!exists(path)) return nullptr;
        auto file = _filesystem.open(path.string());

        return std::make_unique<File>(file.begin(), file.size(), false);
    }

    bool CMRCFileSystem::exists(std::filesystem::path path) {
        return _filesystem.is_file(path.string());
    }
}
