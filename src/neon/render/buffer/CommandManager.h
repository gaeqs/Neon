//
// Created by gaeqs on 16/01/2024.
//

#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include <neon/render/buffer/CommandPool.h>

namespace neon
{
    class Application;
    class CommandPoolHolder;

    class CommandManager
    {
        friend class CommandPoolHolder;

        Application* _application;

        std::vector<CommandPool> _pools;
        std::unordered_map<std::thread::id, CommandPool> _usedPools;
        std::unordered_map<std::thread::id, size_t> _poolCount;
        std::mutex _mutex;

        void freePoolHolder(std::thread::id id);

      public:
        CommandManager(const CommandManager& other) = delete;

        explicit CommandManager(Application* application);

        ~CommandManager() = default;

        CommandPoolHolder fetchCommandPool();

        void disposeAll();
    };

    class CommandPoolHolder
    {
        CommandManager* _manager;
        std::thread::id _threadId;
        bool _valid;

      public:
        CommandPoolHolder(const CommandPoolHolder& other) = delete;

        CommandPoolHolder();

        CommandPoolHolder(CommandManager* manager, std::thread::id id);

        CommandPoolHolder(CommandPoolHolder&& move) noexcept;

        ~CommandPoolHolder() noexcept;

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] CommandPool& getPool() const;

        CommandPoolHolder& operator=(CommandPoolHolder&& move) noexcept;
    };
} // namespace neon

#endif //COMMANDMANAGER_H
