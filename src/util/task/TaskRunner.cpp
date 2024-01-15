//
// Created by gaeqs on 15/01/2024.
//

#include "TaskRunner.h"

#include <iostream>

TaskRunner::TaskRunner() = default;

void TaskRunner::runOnMainThread(std::function<void()> task) {
    _mainThreadMutex.lock();
    _mainThreadTasks.push_back(std::move(task));
    _mainThreadMutex.unlock();
}

void TaskRunner::flushMainThreadTasks() {
    _mainThreadMutex.lock();

    while (!_mainThreadTasks.empty()) {
        try {
            _mainThreadTasks.back()();
        }
        catch (std::exception& ex) {
            std::cerr <<
                    "Error while executing task: " << ex.what() << "." <<
                    std::endl;
        }
        _mainThreadTasks.pop_back();
    }

    _mainThreadMutex.unlock();
}

void TaskRunner::joinAsyncTasks() {
    _mutex.lock();
    for (auto& asyncTask : _tasks) {
        asyncTask->thread.join();
    }
    _tasks.clear();
    _mutex.unlock();
}
