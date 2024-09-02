//
// Created by gaeqs on 15/01/2024.
//

#ifndef TASKRUNNER_H
#define TASKRUNNER_H

#include <algorithm>
#include <condition_variable>
#include <coroutine>
#include <vector>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <utility>

#include <util/task/AbstractCoroutine.h>

namespace neon {
    /**
     * This class hold the status information
     * about a task being executed.
     * <p>
     * Instances of this class may be used to check
     * the status and the result of the represented task.
     * @tparam Result the result type. It may be null.
     */
    template<typename Result>
    class Task {
        std::mutex _valueMutex;
        std::condition_variable _valueCondition;

        std::optional<Result> _result;
        std::atomic_bool _finished;
        std::atomic_bool _cancelled;

    public :
        /**
        * Creates a Task instance.
        */
        Task() :
            _result(),
            _finished(false),
            _cancelled(false) {}

        /**
         * Returns whether this task has finished.
         * @return whether this task has finished.
         */
        [[nodiscard]] bool hasFinished() const {
            return _finished;
        }

        /**
         * Returns whether this task was cancelled.
         * @return whether this task was cancelled.
         */
        [[nodiscard]] bool isCancelled() const {
            return _cancelled;
        }

        /**
         * Suspends the current thread until
         * this task finishes or is cancelled.
         * <p>
         * If this task has already finished, this method does nothing.
         */
        void wait() {
            std::unique_lock lock(_valueMutex);
            if(_finished || _cancelled) return;
            _valueCondition.wait(lock);
        }

        /**
         * Sets the result of the task.
         * @param result the result.
         */
        void setResult(Result&& result) {
            std::unique_lock lock(_valueMutex);
            if(_finished || _cancelled) return;
            _result = std::move(result);
            _finished = true;
            _valueCondition.notify_all();
        }

        /**
         * Cancels the execution of this task.
         * If the task was already running, this
         * function won't be able to stop the execution,
         * but it will stop the assignment of the result.
         */
        void cancel() {
            std::unique_lock lock(_valueMutex);
            if(_finished | _cancelled) return;
            _cancelled = true;
            _valueCondition.notify_all();
        }

        /**
         * Returns a rvalue of the result of the task.
         * <p>
         * Always check if the task has already finished
         * before using the function.
         * <p>
         * This function is not thread-safe.
         *
         * @return the rvalue.
         */
        std::optional<Result>&& moveResult() {
            return std::move(_result);
        }

        /**
         * Returns a reference of the result of the task.
         * <p>
         * Always check if the task has already finished
         * before using the function.
         * <p>
         * This function is not thread-safe.
         *
         * @return the reference.
         */
        std::optional<Result>& getResult() {
            return _result;
        }
    };

    /**
    * This class hold the status information
    * about a task being executed.
    * <p>
    * Instances of this class may be used to check
    * the status and the result of the represented task.
    */
    template<>
    class Task<void> {
        std::mutex _valueMutex;
        std::condition_variable _valueCondition;
        std::atomic_bool _finished;
        std::atomic_bool _cancelled;

    public:
        /**
         * Creates a Task instance.
         */
        Task() :
            _finished(false) {}

        /**
         * Returns whether this task has finished.
         * @return whether this task has finished.
         */
        [[nodiscard]] bool hasFinished() const {
            return _finished;
        }

        /**
         * Returns whether this task was cancelled.
         * @return whether this task was cancelled.
         */
        [[nodiscard]] bool isCancelled() const {
            return _cancelled;
        }

        /**
         * Cancels the execution of this task.
         * If the task was already running, this
         * function won't be able to stop the execution,
         * but it will stop the assignment of the result.
         */
        void cancel() {
            std::unique_lock lock(_valueMutex);
            if(_finished | _cancelled) return;
            _cancelled = true;
            _valueCondition.notify_all();
        }

        /**
         * Marks this task as finished.
         */
        void finish() {
            std::unique_lock lock(_valueMutex);
            if(_finished | _cancelled) return;
            _finished = true;
            _valueCondition.notify_all();
        }

        /**
         * Suspends the current thread until
         * this task finishes.
         * <p>
         * If this task has already finished, this method does nothing.
         */
        void wait() {
            std::unique_lock lock(_valueMutex);
            if(_finished | _cancelled) return;
            _valueCondition.wait(lock);
        }
    };

    /**
     * Instances of this class manages and executes tasks given
     * by the user.
     * <p>
     * You may create TaskRunner instances, but it is recommended
     * to use the TaskRunner locates at the Application instance.
     */
    class TaskRunner {
        std::vector<std::thread> _workers;
        std::queue<std::function<void()>> _pendingTasks;
        std::vector<std::function<void()>> _mainThreadTasks;
        std::vector<std::unique_ptr<AbstractCoroutine>> _coroutines;
        std::mutex _mutex, _coroutineMutex, _mainThreadMutex;
        std::condition_variable _pendingTasksCondition;

        bool _stop;

    public:
        /**
         * Creates a TaskRunner.
         */
        TaskRunner();

        /**
         * Destroys the TaskRunner.
         * <p>
         * This destructor invokes the function shutdown().
         */
        ~TaskRunner();

        /**
         * Shutdowns this task runner, making it unable
         * to receive more requests.
         * <p>
         * This method will wait for all async tasks to finish,
         * but it won't wait for the running coroutines now the
         * functions that would be executed in the main thread.
         */
        void shutdown();

        /**
         * Launches all tasks pending to be executed
         * in the main thread.
         * This includes coroutines.
         * <p>
         * Don't use this method if you don't work
         * the TaskRunner.
         */
        void flushMainThreadTasks();

        /**
         * Stores the given coroutine and schedules
         * it to be launched on the main thread.
         * @tparam Coroutine the coroutine type.
         * @param coroutine the coroutine.
         */
        template<typename Coroutine>
            requires (!std::is_reference_v<Coroutine>)
        void launchCoroutine(Coroutine&& coroutine) {
            if(_stop) return;
            std::unique_ptr<AbstractCoroutine> ptr =
                    std::make_unique<Coroutine>(
                        std::forward<Coroutine&&>(coroutine));

            std::unique_lock lock(_coroutineMutex);
            _coroutines.push_back(std::move(ptr));
        }

        /**
         * Schedules the given function to be launched
         * in the main thread.
         *
         * @tparam Return the return value of the function.
         * @tparam FParams the params of teh function.
         * @param function the function.
         * @param args the arguments passed to the function-
         * @return the task that monitors the state of the function.
         */
        auto executeOnMainThread(auto function,
                                 auto... args) ->
            std::shared_ptr<Task<decltype(function(args...))>> {
            using Return = decltype(function(args...));
            using Tu = std::tuple<decltype(args)...>;
            if(_stop) return nullptr;
            std::shared_ptr task = std::make_shared<Task<Return>>();
            std::shared_ptr tuple = std::make_shared<Tu>(std::move(args)...);

            std::function<void()> run = [
                        task,
                        function,
                        t = std::move(tuple)
                    ] {
                if(task->isCancelled()) return;
                Tu tu = std::move(*t);
                if constexpr(std::is_void_v<Return>) {
                    function(std::move(std::get<decltype(args)>(tu))...);
                    task->finish();
                } else {
                    task->setResult(
                        function(std::move(std::get<decltype(args)>(tu))...));
                }
            };

            std::unique_lock lock(_mainThreadMutex);
            _mainThreadTasks.push_back(std::move(run));

            return task;
        }

        /**
         * Schedules the given function to be launched
         * asynchronously.
         *
         * @tparam Return the return value of the function.
         * @tparam FParams the params of teh function.
         * @param function the function.
         * @param args the arguments passed to the function-
         * @return the task that monitors the state of the function.
         */
        auto executeAsync(auto function,
                          auto... args) ->
            std::shared_ptr<Task<decltype(function(args...))>> {
            using Return = decltype(function(args...));
            using Tu = std::tuple<decltype(args)...>;
            if(_stop) return nullptr;
            std::shared_ptr task = std::make_shared<Task<Return>>();
            std::shared_ptr tuple = std::make_shared<Tu>(std::move(args)...);

            std::function<void()> run = [
                        task,
                        function,
                        t = std::move(tuple)
                    ] {
                if(task->isCancelled()) return;
                Tu tu = std::move(*t);
                if constexpr(std::is_void_v<Return>) {
                    function(std::move(std::get<decltype(args)>(tu))...);
                    task->finish();
                } else {
                    task->setResult(
                        function(std::move(std::get<decltype(args)>(tu))...));
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
