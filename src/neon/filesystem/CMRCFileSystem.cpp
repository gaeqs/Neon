//
// Created by gaeqs on 25/10/24.
//

#include "CMRCFileSystem.h"

namespace neon
{
    CMRCFileSystem::CMRCFileSystem(cmrc::embedded_filesystem filesystem) :
        _filesystem(filesystem)
    {
    }

    std::optional<File> CMRCFileSystem::readFile(std::filesystem::path path) const
    {
        if (!exists(path)) {
            return {};
        }

        auto string = path.lexically_normal().string();
        std::ranges::replace(string, '\\', '/');
        auto file = _filesystem.open(string);

        return File(static_cast<const std::byte*>(static_cast<const void*>(file.begin())), file.size());
    }

    bool CMRCFileSystem::exists(std::filesystem::path path) const
    {
        auto string = path.lexically_normal().string();
        std::ranges::replace(string, '\\', '/');
        return _filesystem.is_file(string);
    }
} // namespace neon
