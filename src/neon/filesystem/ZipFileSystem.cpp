//
// Created by gaeqs on 25/10/24.
//

#include "ZipFileSystem.h"

#include <neon/logging/Logger.h>

neon::ZipFileSystem::ZipFileSystem(const std::filesystem::path& file) {
    _zip = std::make_unique<libzippp::ZipArchive>(file.string());
    if (!_zip->open()) return;
}

neon::ZipFileSystem::~ZipFileSystem() {
    if (!_zip->isOpen()) return;
    _zip->close();
}

std::unique_ptr<neon::File> neon::ZipFileSystem::readFile(std::filesystem::path path) {
    auto file = _zip->getEntry(path.string());
    if (file.isNull() || !file.isFile()) return nullptr;
    return std::make_unique<File>(static_cast<char*>(file.readAsBinary()), file.getSize());
}

bool neon::ZipFileSystem::exists(std::filesystem::path path) {
    auto file = _zip->getEntry(path.string());
    return !file.isNull() && file.isFile();
}
