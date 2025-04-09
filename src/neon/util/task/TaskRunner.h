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
#include <neon/logging/Logger.h>

#include <neon/util/task/AbstractCoroutine.h>

namespace neon
{

    class TaskRunner;

    struct TaskStatus
    {
        std::atomic_bool finished;
        std::atomic_bool cancelled;
    };

    class AbstractTask
    {
      public:
        virtual ~AbstractTask() = default;

        [[nodiscard]] virtual std::shared_ptr<const TaskStatus> getStatus() const = 0;

        [[nodiscard]] virtual bool hasFinished() const = 0;

        [[nodiscard]] virtual bool isCancelled() const = 0;

        virtual void wait() = 0;

        virtual void cancel() = 0;

        virtual void addDependency(const AbstractTask* dependency) = 0;

        virtual bool isUnlocked() const = 0;

        virtual bool isAnyDependencyCancelled() const = 0;

        virtual void provideContext(TaskRunner* runner) = 0;
    };

    /**
     * This class hold the status information
     * about a task being executed.
     * <p>
     * Instances of this class may be used to check
     * the status and the result of the represented task.
     * @tparam Result the result type. It may be null.
     */
    template<typename Result>
    class Task : public AbstractTask
    {
        TaskRunner* _runner;
        std::mutex _valueMutex;
        std::condition_variable _valueCondition;

        std::vector<std::shared_ptr<const TaskStatus>> _dependencies;

        std::shared_ptr<TaskStatus> _status;
        std::optional<Result> _result;

      public:
        /**
         * Creates a Task instance.
         */
        explicit Task(TaskRunner* runner) :
            _runner(runner),
            _result(),
            _status(std::make_shared<TaskStatus>(false, false))
        {
        }

        ~Task() override = default;

        [[nodiscard]] std::shared_ptr<const TaskStatus> getStatus() const override
        {
            return _status;
        }

        /**
         * Returns whether this task has finished.
         * @return whether this task has finished.
         */
        [[nodiscard]] bool hasFinished() const override
        {
            return _status->finished;
        }

        /**
         * Returns whether this task was cancelled.
         * @return whether this task was cancelled.
         */
        [[nodiscard]] bool isCancelled() const override
        {
            return _status->cancelled;
        }

        /**
         * Suspends the current thread until
         * this task finishes or is cancelled.
         * <p>
         * If this task has already finished, this method does nothing.
         */
        void wait() override
        {
            std::unique_lock lock(_valueMutex);
            if (_status->finished || _status->cancelled) {
                return;
            }
            _valueCondition.wait(lock);
        }

        /**
         * Cancels the execution of this task.
         * If the task was already running, this
         * function won't be able to stop the execution,
         * but it will stop the assignment of the result.
         */
        void cancel() override
        {
            std::lock_guard lock(_valueMutex);
            if (_status->finished || _status->cancelled) {
                return;
            }
            _status->cancelled = true;
            _valueCondition.notify_all();
        }

        void addDependency(const AbstractTask* dependency) override
        {
            _dependencies.push_back(dependency->getStatus());
        }

        void provideContext(TaskRunner* runner) override
        {
            _runner = runner;
        }

        [[nodiscard]] bool isUnlocked() const override
        {
            return std::ranges::all_of(_dependencies, [](const auto& dependency) { return dependency->finished.load(); });
        }

        [[nodiscard]] bool isAnyDependencyCancelled() const override
        {
            return std::ranges::any_of(_dependencies, [](const auto& dependency) { return dependency->cancelled.load(); });
        }

        /**
         * Sets the result of the task.
         * @param result the result.
         */
        void setResult(Result&& result)
        {
            std::lock_guard lock(_valueMutex);
            if (_status->finished || _status->cancelled) {
                return;
            }
            _result = std::move(result);
            _status->finished = true;
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
        std::optional<Result>&& moveResult()
        {
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
        std::optional<Result>& getResult()
        {
            return _result;
        }

        template<typename Func, typename... Args>
        auto then(Func&& function, Args&&... args)
            -> std::shared_ptr<Task<decltype(function(std::forward<Args>(args)...))>>;
    };

    /**
     * This class hold the status information
     * about a task being executed.
     * <p>
     * Instances of this class may be used to check
     * the status and the result of the represented task.
     */
    template<>
    class Task<void> : public AbstractTask
    {
        TaskRunner* _runner;
        std::mutex _valueMutex;
        std::condition_variable _valueCondition;

        std::vector<std::shared_ptr<const TaskStatus>> _dependencies;
        std::shared_ptr<TaskStatus> _status;

      public:
        /**
         * Creates a Task instance.
         */
        explicit Task(TaskRunner* runner) :
            _runner(runner),
            _status(std::make_shared<TaskStatus>(false, false))
        {
        }

        [[nodiscard]] std::shared_ptr<const TaskStatus> getStatus() const override
        {
            return _status;
        }

        /**
         * Returns whether this task has finished.
         * @return whether this task has finished.
         */
        [[nodiscard]] bool hasFinished() const override
        {
            return _status->finished;
        }

        /**
         * Returns whether this task was cancelled.
         * @return whether this task was cancelled.
         */
        [[nodiscard]] bool isCancelled() const override
        {
            return _status->cancelled;
        }

        /**
         * Cancels the execution of this task.
         * If the task was already running, this
         * function won't be able to stop the execution,
         * but it will stop the assignment of the result.
         */
        void cancel() override
        {
            std::lock_guard lock(_valueMutex);
            if (_status->finished | _status->cancelled) {
                return;
            }
            _status->cancelled = true;
            _valueCondition.notify_all();
        }

        /**
         * Suspends the current thread until
         * this task finishes.
         * <p>
         * If this task has already finished, this method does nothing.
         */
        void wait() override
        {
            std::unique_lock lock(_valueMutex);
            if (_status->finished || _status->cancelled) {
                return;
            }
            _valueCondition.wait(lock);
        }

        void addDependency(const AbstractTask* dependency) override
        {
            _dependencies.push_back(dependency->getStatus());
        }

        void provideContext(TaskRunner* runner) override
        {
            _runner = runner;
        }

        [[nodiscard]] bool isUnlocked() const override
        {
            return std::ranges::all_of(_dependencies, [](const auto& dependency) { return dependency->finished.load(); });
        }

        [[nodiscard]] bool isAnyDependencyCancelled() const override
        {
            return std::ranges::any_of(_dependencies, [](const auto& dependency) { return dependency->cancelled.load(); });
        }

        /**
         * Marks this task as finished.
         */
        void finish()
        {
            std::lock_guard lock(_valueMutex);
            if (_status->finished || _status->cancelled) {
                return;
            }
            _status->finished = true;
            _valueCondition.notify_all();
        }

        template<typename Func, typename... Args>
        auto then(Func&& function, Args&&... args)
            -> std::shared_ptr<Task<decltype(function(std::forward<Args>(args)...))>>;
    };

    struct RunningTask
    {
        std::shared_ptr<AbstractTask> task;
        std::function<void()> function;
        bool runOnMainThread;
    };

    /**
     * Instances of this class manages and executes tasks given
     * by the user.
     * <p>
     * You may create TaskRunner instances, but it is recommended
     * to use the TaskRunner locates at the Application instance.
     */
    class TaskRunner
    {
        std::vector<std::thread> _workers;
        std::vector<RunningTask> _blockedTasks;
        std::queue<RunningTask> _pendingTasks;
        std::queue<RunningTask> _mainThreadTasks;
        std::vector<std::unique_ptr<AbstractCoroutine>> _coroutines;
        std::mutex _mutex, _blockedMutex, _coroutineMutex, _mainThreadMutex;
        std::condition_variable _pendingTasksCondition;

        bool _stop;

        void manageRunningTaskAddition(RunningTask&& task);

        void checkBlockedTasks();

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
         * Don't use this method if you don't manage
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
            requires(!std::is_reference_v<Coroutine>)
        void launchCoroutine(Coroutine&& coroutine)
        {
            if (_stop) {
                return;
            }
            std::unique_ptr<AbstractCoroutine> ptr = std::make_unique<Coroutine>(std::forward<Coroutine&&>(coroutine));
            ptr->provideContext(this);

            std::lock_guard lock(_coroutineMutex);
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
        template<typename Func, typename... Args>
        auto executeOnMainThread(Func&& function, Args&&... args)
            -> std::shared_ptr<Task<decltype(function(std::forward<Args>(args)...))>>
        {
            using Return = decltype(function(std::forward<Args>(args)...));
            using Tu = std::tuple<std::decay_t<Args>...>;
            if (_stop) {
                return nullptr;
            }
            std::shared_ptr task = std::make_shared<Task<Return>>(this);
            std::shared_ptr tuple = std::make_shared<Tu>(std::forward<Args>(args)...);

            RunningTask running{.task = task, .runOnMainThread = true};

            running.function = [task, function, t = std::move(tuple)] {
                if (task->isCancelled()) {
                    return;
                }
                Tu tu = std::move(*t);
                if constexpr (std::is_void_v<Return>) {
                    function(std::move(std::get<std::decay_t<Args>>(tu))...);
                    task->finish();
                } else {
                    task->setResult(function(std::move(std::get<std::decay_t<Args>>(tu))...));
                }
            };

            manageRunningTaskAddition(std::move(running));

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
        template<typename Func, typename... Args>
        auto executeAsync(Func&& function, Args&&... args)
            -> std::shared_ptr<Task<decltype(function(std::forward<Args>(args)...))>>
        {
            using Return = decltype(function(std::forward<Args>(args)...));
            using Tu = std::tuple<std::decay_t<Args>...>;
            if (_stop) {
                return nullptr;
            }
            std::shared_ptr task = std::make_shared<Task<Return>>(this);
            std::shared_ptr tuple = std::make_shared<Tu>(std::forward<Args>(args)...);

            RunningTask running{.task = task, .runOnMainThread = false};

            running.function = [task, function, t = std::move(tuple)] {
                if (task->isCancelled()) {
                    return;
                }
                Tu tu = std::move(*t);
                if constexpr (std::is_void_v<Return>) {
                    function(std::move(std::get<std::decay_t<Args>>(tu))...);
                    task->finish();
                } else {
                    task->setResult(function(std::move(std::get<std::decay_t<Args>>(tu))...));
                }
            };

            manageRunningTaskAddition(std::move(running));

            return task;
        }

        /**
         * Schedules the given function to be launched
         * asynchronously.
         *
         * The result will be stored in the given task.
         *
         * @tparam Return the return value of the function.
         * @tparam FParams the params of teh function.
         * @param task the task that will manage the execution's result.
         * @param function the function.
         * @param args the arguments passed to the function-
         * @return the task that monitors the state of the function.
         */
        template<typename Return, typename Func, typename... Args>
        void executeAsync(std::shared_ptr<Task<Return>> task, Func&& function, Args&&... args)
        {
            using Tu = std::tuple<std::decay_t<Args>...>;
            if (_stop) {
                return;
            }
            std::shared_ptr tuple = std::make_shared<Tu>(std::forward<Args>(args)...);

            RunningTask running{.task = task, .runOnMainThread = false};

            running.function = [task, function, t = std::move(tuple)] {
                if (task->isCancelled()) {
                    return;
                }
                Tu tu = std::move(*t);
                if constexpr (std::is_void_v<Return>) {
                    function(std::move(std::get<std::decay_t<Args>>(tu))...);
                    task->finish();
                } else {
                    task->setResult(function(std::move(std::get<std::decay_t<Args>>(tu))...));
                }
            };

            manageRunningTaskAddition(std::move(running));
        }
    };

    template<typename Result>
    template<typename Func, typename... Args>
    auto Task<Result>::then(Func&& function, Args&&... args)
        -> std::shared_ptr<Task<decltype(function(std::forward<Args>(args)...))>>
    {
        using Return = decltype(function(std::forward<Args>(args)...));
        std::shared_ptr<Task<Return>> task = std::make_shared<Task<Return>>(_runner);
        task->addDependency(this);

        {
            std::lock_guard lock(_valueMutex);
            if (_status->cancelled) {
                task->cancel();
                return task;
            }
        }

        if (_runner == nullptr) {
            error() << "This task has no context. If this is a coroutine's task, launch it before using this function.";
            return nullptr;
        }

        _runner->executeAsync(task, function, std::forward<Args>(args)...);
        return task;
    }

    template<typename Func, typename... Args>
    auto Task<void>::then(Func&& function, Args&&... args)
        -> std::shared_ptr<Task<decltype(function(std::forward<Args>(args)...))>>
    {
        using Return = decltype(function(std::forward<Args>(args)...));
        std::shared_ptr<Task<Return>> task = std::make_shared<Task<Return>>(_runner);
        task->addDependency(this);

        {
            std::lock_guard lock(_valueMutex);
            if (_status->cancelled) {
                task->cancel();
                return task;
            }
        }

        if (_runner == nullptr) {
            error() << "This task has no context. If this is a coroutine's task, launch it before using this function.";
            return nullptr;
        }

        _runner->executeAsync(task, function, std::forward<Args>(args)...);
        return task;
    }
} // namespace neon

#endif // TASKRUNNER_H
