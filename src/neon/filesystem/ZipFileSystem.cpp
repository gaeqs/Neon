//
// Created by gaeqs on 25/10/24.
//

#include "ZipFileSystem.h"

#include <neon/logging/Logger.h>

namespace neon
{
    ZipFileSystem::ZipFileSystem(const std::filesystem::path& file)
    {
        _zip = std::make_unique<libzippp::ZipArchive>(file.string());
        if (!_zip->open()) {
            return;
        }
    }

    ZipFileSystem::~ZipFileSystem()
    {
        if (!_zip->isOpen()) {
            return;
        }
        _zip->close();
    }

    std::optional<File> ZipFileSystem::readFile(std::filesystem::path path) const
    {
        auto file = _zip->getEntry(path.lexically_normal().string());
        if (file.isNull() || !file.isFile()) {
            return {};
        }
        libzippp_uint8* data = file.readAsBinary();
        return File(reinterpret_cast<const char*>(data), file.getSize());
    }

    bool ZipFileSystem::exists(std::filesystem::path path) const
    {
        auto file = _zip->getEntry(path.lexically_normal().string());
        return !file.isNull() && file.isFile();
    }
} // namespace neon
