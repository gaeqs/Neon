//
// Created by gaeqs on 17/01/2024.
//

#include "AssimpNewIOSystem.h"

#include <utility>

namespace fs = std::filesystem;


neon::assimp_loader::AssimpNewIOSystem::AssimpNewIOSystem(
    std::filesystem::path root) : _root(std::move(root)) {
}

bool neon::assimp_loader::AssimpNewIOSystem::Exists(const char* pFile) const {
    auto path = _root / std::string(pFile);
    auto status = fs::status(path);
    return status.type() == fs::file_type::regular;
}

char neon::assimp_loader::AssimpNewIOSystem::getOsSeparator() const {
    return fs::path::preferred_separator;
}

Assimp::IOStream* neon::assimp_loader::AssimpNewIOSystem::Open(
    const char* pFile, const char* pMode) {
}
