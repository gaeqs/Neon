//
// Created by gaeqs on 16/01/2024.
//

#include "CommandManager.h"

#include <iostream>
#include <engine/render/CommandPool.h>

namespace neon {
    void CommandManager::freePoolHolder(std::thread::id id) {
        _mutex.lock();
        auto& count = _poolCount.at(id);
        if (count <= 1) {
            _poolCount.erase(id);
            _pools.push_back(std::move(_usedPools.at(id)));
            _usedPools.erase(id);
        }
        else {
            count--;
        }
        _mutex.unlock();
    }

    CommandManager::CommandManager(Application* application)
        : _application(application) {
    }

    CommandPoolHolder CommandManager::fetchCommandPool() {
        auto id = std::this_thread::get_id();

        _mutex.lock();

        if (_usedPools.contains(id)) {
            // Pool found for the current thread.
            _poolCount[id]++;
        }
        else {
            // Check if there's any command pool free.
            if (_pools.empty()) {
                // There's no pools free. Create a new pool.
                _usedPools.emplace(id, CommandPool(_application));
            }
            else {
                _usedPools.emplace(id, std::move(_pools.back()));
                _pools.pop_back();
            }
            _poolCount.emplace(id, 1);
        }

        _mutex.unlock();
        return {this, id};
    }

    void CommandManager::disposeAll() {
        _pools.clear();
        _usedPools.clear();
        _poolCount.clear();
    }

    CommandPoolHolder::CommandPoolHolder()
        : _manager(nullptr),
          _valid(false) {
    }

    CommandPoolHolder::CommandPoolHolder(CommandManager* manager,
                                         std::thread::id threadId)
        : _manager(manager),
          _threadId(threadId),
          _valid(true) {
    }

    CommandPoolHolder::CommandPoolHolder(CommandPoolHolder&& move) noexcept
        : _manager(move._manager),
          _threadId(move._threadId),
          _valid(move._valid) {
        move._valid = false;
    }

    CommandPoolHolder::~CommandPoolHolder() noexcept {
        if (_valid) {
            _manager->freePoolHolder(_threadId);
        }
    }

    CommandPool& CommandPoolHolder::getPool() const {
        return _manager->_usedPools.at(_threadId);
    }

    CommandPoolHolder& CommandPoolHolder::operator=(
        CommandPoolHolder&& move) noexcept {
        if (this == &move) return *this; // SAME!
        if (_valid) {
            _manager->freePoolHolder(_threadId);
        }
        _manager = move._manager;
        _threadId = move._threadId;
        _valid = move._valid;
        move._valid = false;
        return *this;
    }
}
