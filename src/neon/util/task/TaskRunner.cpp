//
// Created by gaeqs on 15/01/2024.
//

#include "TaskRunner.h"

#include <neon/logging/Logger.h>

namespace neon {
    TaskRunner::TaskRunner(): _stop(false) {
        uint32_t threads = std::max(std::thread::hardware_concurrency(), 2u) -
                           1u;
        _workers.reserve(threads);

        for (uint32_t i = 0; i < threads; ++i) {
            _workers.emplace_back([this] {
                std::function<void()> task;
                while (true) {
                    {
                        std::unique_lock lock(_mutex);
                        _pendingTasksCondition.wait(lock, [this] {
                            return !_pendingTasks.empty() || _stop;
                        });

                        if (_stop && _pendingTasks.empty()) return;

                        task = std::move(_pendingTasks.front());
                        _pendingTasks.pop();
                    }
                    task();
                }
            });
        }
    }

    TaskRunner::~TaskRunner() {
        shutdown();
    }

    void TaskRunner::shutdown() {
        if (_stop) return;
        //
        {
            std::lock_guard lock(_mutex);
            _stop = true;
        }
        _pendingTasksCondition.notify_all();
        for (auto& worker: _workers) {
            worker.join();
        }
    }

    void TaskRunner::flushMainThreadTasks() {
        if (_stop) return;
        //
        {
            std::lock_guard lock(_coroutineMutex);
            std::erase_if(
                _coroutines,
                [](const auto& it) {
                    return it->isDone();
                }
            );

            for (auto& coroutine: _coroutines) {
                if (coroutine->isReady()) {
                    coroutine->launch();
                }
            }
        }

        std::lock_guard lock(_mainThreadMutex);

        while (!_mainThreadTasks.empty()) {
            try {
                _mainThreadTasks.back()();
            } catch (std::exception& ex) {
                Logger::defaultLogger()->error(MessageBuilder()
                    .print("Error while executing task: ")
                    .print(ex.what(), TextEffect::foreground4bits(1))
                    .print("."));
            }
            _mainThreadTasks.pop_back();
        }
    }
}
