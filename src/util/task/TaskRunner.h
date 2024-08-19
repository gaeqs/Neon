//
// Created by gaeqs on 15/01/2024.
//

#ifndef TASKRUNNER_H
#define TASKRUNNER_H

#include <condition_variable>
#include <vector>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

template<typename Result>
class Task {
    std::atomic_bool _finished;
    std::mutex _valueMutex;
    std::condition_variable _valueCondition;

    std::optional<Result> _result;

public :
    Task() :
        _finished(false),
        _result() {
    }

    ~Task();

    bool hasFinished();

    void wait();

    const std::optional<Result>& getResult() {
        return _result;
    }

    void setResult(Result&& result) { {
            std::lock_guard lock(_valueMutex);
            _result = std::move(result);
            _finished = true;
        }
        _valueCondition.notify_all();
    }
};

template<>
class Task<void> {
    bool hasFinished();

    void wait();
};

class TaskRunner {
    struct AsyncTask {
        std::thread thread;
        std::atomic_bool finished = false;
    };

    std::vector<std::unique_ptr<AsyncTask>> _tasks;
    std::vector<std::function<void()>> _mainThreadTasks;
    std::mutex _mutex, _mainThreadMutex;

public:
    TaskRunner();

    void runOnMainThread(std::function<void()> task);

    void flushMainThreadTasks();

    template<class Fn, class... Param>
    void executeAsync(Fn&& function, Param&&... args) {
        auto task = std::make_unique<AsyncTask>();
        auto raw = task.get();
        task->thread = std::thread([=] {
            function(args...);
            raw->finished = true;
        });
        _mutex.lock();
        std::erase_if(_tasks, [](const std::unique_ptr<AsyncTask>& t) {
            if (t->finished) {
                t->thread.join();
                return true;
            }
            return false;
        });
        _tasks.push_back(std::move(task));
        _mutex.unlock();
    }

    void joinAsyncTasks();
};


#endif //TASKRUNNER_H
