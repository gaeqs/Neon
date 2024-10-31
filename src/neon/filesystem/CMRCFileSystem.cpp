//
// Created by gaeqs on 25/10/24.
//

#include "CMRCFileSystem.h"


namespace neon {
    CMRCFileSystem::CMRCFileSystem(cmrc::embedded_filesystem filesystem)
        : _filesystem(filesystem) {}

    std::optional<File> CMRCFileSystem::readFile(std::filesystem::path path) {
        if (!exists(path)) return {};

        auto string = path.lexically_normal().string();
        std::ranges::replace(string, '\\', '/');
        auto file = _filesystem.open(string);

        return File(file.begin(), file.size(), false);
    }

    bool CMRCFileSystem::exists(std::filesystem::path path) {
        auto string = path.lexically_normal().string();
        std::ranges::replace(string, '\\', '/');
        return _filesystem.is_file(string);
    }
}
