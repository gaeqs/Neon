//
// Created by gaeqs on 17/01/2024.
//

#ifndef ASSIMPNEWIOSYSTEM_H
#define ASSIMPNEWIOSYSTEM_H

#include <filesystem>
#include <assimp/DefaultIOSystem.h>

namespace neon::assimp_loader {
    class AssimpNewIOSystem : Assimp::IOSystem {
        std::filesystem::path _root;

    public:
        explicit AssimpNewIOSystem(std::filesystem::path root);

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
