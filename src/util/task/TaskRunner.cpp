//
// Created by gaeqs on 15/01/2024.
//

#include "TaskRunner.h"

#include <iostream>

namespace neon {
    TaskRunner::TaskRunner(): _stop(false) {
        uint32_t threads = std::min(std::thread::hardware_concurrency(), 2u) -
                           1u;
        _workers.reserve(threads);

        for(uint32_t i = 0; i < threads; ++i) {
            _workers.emplace_back([this] {
                std::function<void()> task;
                while(true) {
                    {
                        std::unique_lock lock(_mutex);
                        _pendingTasksCondition.wait(lock, [this] {
                            return !_pendingTasks.empty() || _stop;
                        });

                        if(_stop && _pendingTasks.empty()) return;

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
        if(_stop) return;
        //
        {
            std::unique_lock lock(_mutex);
            _stop = true;
        }
        _pendingTasksCondition.notify_all();
        for(auto& worker: _workers) {
            worker.join();
        }
    }

    void TaskRunner::runOnMainThread(std::function<void()> task) {
        std::unique_lock lock(_mainThreadMutex);
        _mainThreadTasks.push_back(std::move(task));
    }

    void TaskRunner::flushMainThreadTasks() {
        //
        {
            std::unique_lock lock(_coroutineMutex);
            std::erase_if(
                _coroutines,
                [](const Coroutine& it) {
                    return it.isDone();
                }
            );

            for(auto& coroutine: _coroutines) {
                if(coroutine.isReady()) {
                    coroutine.launch();
                }
            }
        }

        std::unique_lock lock(_mainThreadMutex);

        while(!_mainThreadTasks.empty()) {
            try {
                _mainThreadTasks.back()();
            } catch(std::exception& ex) {
                std::cerr <<
                        "Error while executing task: " << ex.what() << "." <<
                        std::endl;
            }
            _mainThreadTasks.pop_back();
        }
    }

    void TaskRunner::launchCoroutine(Coroutine&& coroutine) {
        std::unique_lock lock(_coroutineMutex);
        _coroutines.push_back(std::move(coroutine));
    }
}
