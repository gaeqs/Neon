//
// Created by gaeqs on 17/01/2024.
//

#include "AssimpNewIOSystem.h"

#include <iostream>
#include <assimp/DefaultIOStream.h>

#include <utility>


namespace fs = std::filesystem;

namespace {
    std::ios_base::openmode translateOpenMode(const char* mode) {
        std::ios_base::openmode result = std::ios_base::binary;
        while (*mode) {
            switch (*mode) {
                case 'r':
                    result |= std::ios_base::in;
                    break;
                case 'w':
                    result |= std::ios_base::out | std::ios_base::trunc;
                    break;
                case 'a':
                    result |= std::ios_base::out | std::ios_base::app;
                    break;
                case 'b':
                    result |= std::ios_base::binary;
                    break;
                case '+':
                    result |= std::ios_base::in | std::ios_base::out;
                    break;
                default:
                    break;
            }
            ++mode;
        }
        return result;
    }
}

namespace neon::assimp_loader {
    AssimpNewIOStream::AssimpNewIOStream(std::filesystem::path path,
                                         const char* mode)
        : _path(path),
          _stream(path, translateOpenMode(mode)) {
    }

    size_t AssimpNewIOStream::Read(void* pvBuffer,
                                   size_t pSize, size_t pCount) {
        _stream.read(static_cast<char*>(pvBuffer), pSize * pCount);
        return _stream.gcount();
    }

    size_t AssimpNewIOStream::Write(const void* pvBuffer,
                                    size_t pSize, size_t pCount) {
        _stream.write(static_cast<const char*>(pvBuffer),
            pSize * pCount);
        return pCount;
    }

    aiReturn AssimpNewIOStream::Seek(size_t pOffset, aiOrigin pOrigin) {
        _stream.seekg(pOffset, static_cast<std::ios_base::seekdir>(pOrigin));
        return aiReturn_SUCCESS;
    }

    size_t AssimpNewIOStream::Tell() const {
        return _stream.tellg();
    }

    size_t AssimpNewIOStream::FileSize() const {
        return fs::file_size(_path);
    }

    void AssimpNewIOStream::Flush() {
        _stream.flush();
    }

    AssimpNewIOSystem::AssimpNewIOSystem(
        std::filesystem::path root)
        : _root(std::move(root)),
          _rootName(_root.string()) {
    }

    bool AssimpNewIOSystem::Exists(const char* pFile) const {
        auto path = _root / std::string(pFile);
        auto abs = fs::absolute(path);
auto exists = fs::is_regular_file(path);
        return fs::is_regular_file(path);
    }

    char AssimpNewIOSystem::getOsSeparator() const {
        return fs::path::preferred_separator;
    }

    Assimp::IOStream* AssimpNewIOSystem::Open(const char* pFile,
                                              const char* pMode) {
        return new AssimpNewIOStream(_root / std::string(pFile), pMode);
    }

    void AssimpNewIOSystem::Close(Assimp::IOStream* pFile) {
        delete pFile;
    }

    bool AssimpNewIOSystem::ComparePaths(const char* one,
                                         const char* second) const {
        return fs::equivalent(
            _root / std::string(one),
            _root / std::string(second)
        );
    }

    bool AssimpNewIOSystem::PushDirectory(const std::string& path) {
        return IOSystem::PushDirectory(path);
    }

    const std::string& AssimpNewIOSystem::CurrentDirectory() const {
        return _rootName;
    }

    size_t AssimpNewIOSystem::StackSize() const {
        return IOSystem::StackSize();
    }

    bool AssimpNewIOSystem::PopDirectory() {
        return IOSystem::PopDirectory();
    }

    bool AssimpNewIOSystem::CreateDirectory(const std::string& path) {
        auto abs = _root / std::string(path);
        return fs::create_directories(abs);
    }

    bool AssimpNewIOSystem::ChangeDirectory(const std::string& path) {
        _root = fs::path(path);
        _rootName = _root.string();
        return true;
    }

    bool AssimpNewIOSystem::DeleteFile(const std::string& file) {
        auto abs = _root / std::string(file);
        return fs::remove(abs);
    }
}
