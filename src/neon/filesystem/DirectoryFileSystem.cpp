//
// Created by gaeqs on 24/10/24.
//

#include "DirectoryFileSystem.h"

#include <fstream>
#include <utility>

namespace neon {
    DirectoryFileSystem::DirectoryFileSystem(std::filesystem::path root)
        : _root(std::move(root)) {}

    std::optional<File> DirectoryFileSystem::readFile(std::filesystem::path path) {
        auto result = _root / path;
        if (!std::filesystem::exists(result) || is_directory(result)) return {};

        std::ifstream file(result, std::ios::binary | std::ios::ate);
        std::ifstream::pos_type pos = file.tellg();

        auto data = new char[pos];

        file.seekg(0, std::ios::beg);
        file.read(data, pos);

        return File(data, pos);
    }

    bool DirectoryFileSystem::exists(std::filesystem::path path) {
        auto result = _root / path;
        return std::filesystem::exists(result) && !is_directory(result);
    }
}
