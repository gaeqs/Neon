//
// Created by gaeqs on 15/01/2024.
//

#include "TaskRunner.h"

#include <neon/logging/Logger.h>

namespace neon
{
    TaskRunner::TaskRunner() :
        _stop(false)
    {
        uint32_t threads = std::max(std::thread::hardware_concurrency(), 2u) - 1u;
        _workers.reserve(threads);

        for (uint32_t i = 0; i < threads; ++i) {
            _workers.emplace_back([this] {
                RunningTask task;
                while (true) {
                    {
                        std::unique_lock lock(_mutex);
                        _pendingTasksCondition.wait(lock, [this] { return !_pendingTasks.empty() || _stop; });

                        if (_stop && _pendingTasks.empty()) {
                            return;
                        }

                        task = std::move(_pendingTasks.front());
                        _pendingTasks.pop();
                    }

                    try {
                        task.function();
                    } catch (std::exception& ex) {
                        logger.error(MessageBuilder()
                                         .print("Error while executing task: ")
                                         .print(ex.what(), TextEffect::foreground4bits(1))
                                         .print("."));
                        task.task->cancel();
                    }

                    checkBlockedTasks();
                }
            });
        }
    }

    TaskRunner::~TaskRunner()
    {
        shutdown();
    }

    void TaskRunner::manageRunningTaskAddition(RunningTask&& task)
    {
        if (task.task->isAnyDependencyCancelled() || task.task->isCancelled()) {
            task.task->cancel();
            return;
        }

        if (!task.task->isUnlocked()) {
            std::lock_guard lock(_blockedMutex);
            _blockedTasks.push_back(std::move(task));
            return;
        }

        if (task.runOnMainThread) {
            std::lock_guard lock(_mainThreadMutex);
            _mainThreadTasks.push(std::move(task));
            return;
        }

        std::lock_guard lock(_mutex);
        _pendingTasks.push(std::move(task));
        _pendingTasksCondition.notify_one();
    }

    void TaskRunner::checkBlockedTasks()
    {
        std::lock_guard lock(_blockedMutex);

        for (auto it = _blockedTasks.begin(); it != _blockedTasks.end();) {
            auto& task = *it;
            if (task.task->isCancelled() || task.task->isAnyDependencyCancelled()) {
                task.task->cancel();
                it = _blockedTasks.erase(it);
                continue;
            }

            if (task.task->isUnlocked()) {
                if (task.runOnMainThread) {
                    std::lock_guard pushLock(_mainThreadMutex);
                    _mainThreadTasks.push(std::move(task));
                } else {
                    std::lock_guard pushLock(_mutex);
                    _pendingTasks.push(std::move(task));
                    _pendingTasksCondition.notify_one();
                }
                it = _blockedTasks.erase(it);

                continue;
            }

            ++it;
        }
    }

    void TaskRunner::shutdown()
    {
        if (_stop) {
            return;
        }
        //
        {
            std::lock_guard lock(_mutex);
            _stop = true;
        }
        _pendingTasksCondition.notify_all();
        for (auto& worker : _workers) {
            worker.join();
        }
    }

    void TaskRunner::flushMainThreadTasks()
    {
        if (_stop) {
            return;
        }
        {
            std::lock_guard lock(_coroutineMutex);
            std::erase_if(_coroutines, [](const auto& it) { return it->isDone(); });

            for (auto& coroutine : _coroutines) {
                if (coroutine->isReady()) {
                    coroutine->launch();
                }
            }
        }
        {
            std::lock_guard lock(_mainThreadMutex);
            while (!_mainThreadTasks.empty()) {
                RunningTask task;
                try {
                    task = std::move(_mainThreadTasks.front());
                    _mainThreadTasks.pop();
                    task.function();
                } catch (std::exception& ex) {
                    logger.error(MessageBuilder()
                                     .print("Error while executing task: ")
                                     .print(ex.what(), TextEffect::foreground4bits(1))
                                     .print("."));
                    task.task->cancel();
                }
            }
        }
        checkBlockedTasks();
    }
} // namespace neon
