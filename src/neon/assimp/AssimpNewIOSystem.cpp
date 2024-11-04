//
// Created by gaeqs on 17/01/2024.
//

#include "AssimpNewIOSystem.h"

#include <assimp/DefaultIOStream.h>

#include <utility>


namespace fs = std::filesystem;

namespace neon::assimp_loader {
    AssimpNewIOStream::AssimpNewIOStream(File file)
        : _file(std::move(file)),
          _pointer(0) {}

    size_t AssimpNewIOStream::Read(void* pvBuffer, size_t pSize, size_t pCount) {
        if (_file.getSize() <= _pointer) return 0;
        size_t bytesLeft = _file.getSize() - _pointer;
        size_t elementsLeft = bytesLeft / pSize;
        if (elementsLeft == 0) return 0;
        size_t elementsToRead = std::min(elementsLeft, pCount);
        size_t bytesToRead = elementsToRead * pSize;

        std::memcpy(pvBuffer, _file.getData(), bytesToRead);
        _pointer += bytesToRead;

        return elementsToRead;
    }

    size_t AssimpNewIOStream::Write(const void* pvBuffer,
                                    size_t pSize, size_t pCount) {
        return 0;
    }

    aiReturn AssimpNewIOStream::Seek(size_t pOffset, aiOrigin pOrigin) {
        switch (pOrigin) {
            case aiOrigin_SET:
                _pointer = pOffset;
                break;
            case aiOrigin_CUR:
                _pointer += static_cast<int32_t>(pOffset);
                break;
            case aiOrigin_END:
                _pointer = _file.getSize() + static_cast<int32_t>(pOffset);
                break;
            default:
                break;
        }
        _pointer = std::min(_pointer, _file.getSize());
        return AI_SUCCESS;
    }

    size_t AssimpNewIOStream::Tell() const {
        return _pointer;
    }

    size_t AssimpNewIOStream::FileSize() const {
        return _file.getSize();
    }

    void AssimpNewIOStream::Flush() {}

    AssimpNewIOSystem::AssimpNewIOSystem(
        const FileSystem* fileSystem, std::filesystem::path root)
        : _fileSystem(fileSystem),
          _root(std::move(root)),
          _rootName(_root.string()) {}

    bool AssimpNewIOSystem::Exists(const char* pFile) const {
        return _fileSystem->exists(_root / std::string(pFile));
    }

    char AssimpNewIOSystem::getOsSeparator() const {
        return '/';
    }

    Assimp::IOStream* AssimpNewIOSystem::Open(const char* pFile,
                                              const char* pMode) {
        if (auto file = _fileSystem->readFile(_root / std::string(pFile)); file.has_value()) {
            return new AssimpNewIOStream(std::move(file.value()));
        }
        return nullptr;
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
        return false;
    }

    bool AssimpNewIOSystem::ChangeDirectory(const std::string& path) {
        _root = fs::path(path);
        _rootName = _root.string();
        return true;
    }

    bool AssimpNewIOSystem::DeleteFile(const std::string& file) {
        return false;
    }
}
