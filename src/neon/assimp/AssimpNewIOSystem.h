//
// Created by gaeqs on 17/01/2024.
//

#ifndef ASSIMPNEWIOSYSTEM_H
#define ASSIMPNEWIOSYSTEM_H

#include <filesystem>
#include <fstream>

#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>
#include <neon/filesystem/FileSystem.h>

namespace neon::assimp_loader {
    class AssimpNewIOStream : public Assimp::IOStream {
        File _file;
        size_t _pointer;

    public:
        AssimpNewIOStream(File file);

        inline ~AssimpNewIOStream() override = default;

        size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override;

        size_t Write(const void* pvBuffer, size_t pSize,
                     size_t pCount) override;

        aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override;

        size_t Tell() const override;

        size_t FileSize() const override;

        void Flush() override;
    };

    class AssimpNewIOSystem : public Assimp::IOSystem {
        const FileSystem* _fileSystem;
        std::optional<std::filesystem::path> _root;
        std::string _rootName;

    public:
        explicit AssimpNewIOSystem(const FileSystem* fileSystem);

        AssimpNewIOSystem(const FileSystem* fileSystem, std::filesystem::path root);

        ~AssimpNewIOSystem() override = default;

        bool Exists(const char* pFile) const override;

        [[nodiscard]] char getOsSeparator() const override;

        Assimp::IOStream* Open(const char* pFile, const char* pMode) override;

        void Close(Assimp::IOStream* pFile) override;

        bool ComparePaths(const char* one, const char* second) const override;

        inline bool PushDirectory(const std::string& path) override;

        const std::string& CurrentDirectory() const override;

        inline size_t StackSize() const override;

        inline bool PopDirectory() override;

        inline bool CreateDirectory(const std::string& path) override;

        inline bool ChangeDirectory(const std::string& path) override;

        inline bool DeleteFile(const std::string& file) override;
    };
}


#endif //ASSIMPNEWIOSYSTEM_H
