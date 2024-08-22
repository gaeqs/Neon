//
// Created by gaeqs on 15/01/2024.
//

#ifndef TASKRUNNER_H
#define TASKRUNNER_H

#include <condition_variable>
#include <coroutine>
#include <vector>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <utility>

#include <util/task/Coroutine.h>

namespace neon {
    template<typename Result>
    class Task {
        std::atomic_bool _finished;
        std::mutex _valueMutex;
        std::condition_variable _valueCondition;

        std::optional<Result> _result;

    public :
        Task() :
            _finished(false),
            _result() {}

        bool hasFinished() {
            return _finished;
        }

        void wait() {
            if(_finished) return;
            std::unique_lock lock(_valueMutex);
            _valueCondition.wait(lock);
        }

        const std::optional<Result>& getResult() {
            return _result;
        }

        void setResult(Result&& result) {
            std::unique_lock lock(_valueMutex);
            _result = std::move(result);
            _finished = true;
            _valueCondition.notify_all();
        }
    };

    template<>
    class Task<void> {
        std::atomic_bool _finished;
        std::mutex _valueMutex;
        std::condition_variable _valueCondition;

    public:
        Task() = default;

        bool hasFinished() {
            return _finished;
        }

        void wait() {
            if(_finished) return;
            std::unique_lock lock(_valueMutex);
            _valueCondition.wait(lock);
        }

        void finish() {
            _finished = true;
            _valueCondition.notify_all();
        }
    };

    class TaskRunner {
        std::vector<std::thread> _workers;
        std::queue<std::function<void()>> _pendingTasks;
        std::vector<std::function<void()>> _mainThreadTasks;
        std::vector<Coroutine> _coroutines;
        std::mutex _mutex, _coroutineMutex, _mainThreadMutex;
        std::condition_variable _pendingTasksCondition;

        bool _stop;

    public:
        TaskRunner();

        ~TaskRunner();

        void shutdown();

        void runOnMainThread(std::function<void()> task);

        void flushMainThreadTasks();

        void launchCoroutine(Coroutine&& coroutine);

        template<typename Return, typename... Params>
        std::shared_ptr<Task<Return>>
        executeAsync(const std::function<Return(Params...)>& function,
                     Params... args) {
            if(_stop) return nullptr;
            std::shared_ptr<Task<Return>> task = std::make_shared<Task<
                Return>>();


            std::function<void()> run = [
                        task,
                        function,
                        ...a = std::move(args)
                    ] {
                if constexpr(std::is_void_v<Return>) {
                    function(a...);
                    task->finish();
                } else {
                    task->setResult(function(a...));
                }
            };

            std::unique_lock lock(_mutex);
            _pendingTasks.push(std::move(run));
            _pendingTasksCondition.notify_one();

            return task;
        }

        template<typename Return, typename... Params>
        std::shared_ptr<Task<Return>>
        executeAsync(const std::function<Return(Params...)>& function,
                     Params&&... args) {
            if(_stop) return nullptr;
            std::shared_ptr<Task<Return>> task = std::make_shared<Task<
                Return>>();


            std::function<void()> run = [
                        task,
                        function,
                        ...a = std::forward<Params>(args)
                    ] {
                if constexpr(std::is_void_v<Return>) {
                    function(a...);
                    task->finish();
                } else {
                    task->setResult(function(a...));
                }
            };

            std::unique_lock lock(_mutex);
            _pendingTasks.push(std::move(run));
            _pendingTasksCondition.notify_one();

            return task;
        }
    };
}

#endif //TASKRUNNER_H
