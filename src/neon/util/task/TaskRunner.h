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

    /**
     * Structure that holds the status of a task.
     *
     * This structure is used by task implementations to indicate if a task has
     * finished execution or has been cancelled.
     */
    struct TaskStatus
    {
        std::atomic_bool finished;
        std::atomic_bool cancelled;
    };

    /**
     * Abstract base class for all tasks.
     *
     * This class defines the common interface for tasks,
     * including status queries, waiting for completion,
     * cancellation, dependency management, and context provision.
     */
    class AbstractTask
    {
      public:
        virtual ~AbstractTask() = default;

        /**
         * Retrieves the immutable status of the task.
         *
         * @return Shared pointer to the task's status information.
         */
        [[nodiscard]] virtual std::shared_ptr<const TaskStatus> getStatus() const = 0;

        /**
         * Checks if the task has finished execution.
         *
         * @return True if the task is finished.
         */
        [[nodiscard]] virtual bool hasFinished() const = 0;

        /**
         * Checks if the task has been cancelled.
         *
         * @return True if the task is cancelled.
         */
        [[nodiscard]] virtual bool isCancelled() const = 0;

        /**
         * Blocks the calling thread until the task completes or is cancelled.
         */
        virtual void wait() = 0;

        /**
         * Cancels the execution of the task.
         *
         * If the task is already running, cancellation does not interrupt execution,
         * but prevents assignment of the result.
         */
        virtual void cancel() = 0;

        /**
         * Adds a dependency for this task.
         *
         * The dependency can optionally ignore cancellations based on the flag.
         *
         * @param ignoreCancellations If true, cancellation of the dependency is ignored.
         * @param dependency Pointer to the task on which this task depends.
         */
        virtual void addDependency(bool ignoreCancellations, const AbstractTask* dependency) = 0;

        /**
         * Checks if the task is free to execute (i.e. all dependencies are satisfied).
         *
         * @return True if all dependencies have either finished or are cancelled (and ignored).
         */
        [[nodiscard]] virtual bool isUnlocked() const = 0;

        /**
         * Checks if any of the dependencies have been cancelled.
         *
         * @return True if at least one dependency was cancelled (and cancellations are not ignored).
         */
        [[nodiscard]] virtual bool isAnyDependencyCancelled() const = 0;

        /**
         * Provides the task with its execution context.
         *
         * This is used to assign the task runner context to the task.
         *
         * @param runner Pointer to the TaskRunner managing the task.
         */
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
        using HoldValue = std::conditional_t<std::is_void_v<Result>, std::monostate, Result>;

        TaskRunner* _runner;
        std::mutex _valueMutex;
        std::condition_variable _valueCondition;

        std::vector<std::pair<bool, std::shared_ptr<const TaskStatus>>> _dependencies;

        std::shared_ptr<TaskStatus> _status;
        std::optional<HoldValue> _result;

      public:
        /**
         * Constructs a new Task instance.
         *
         * @param runner Pointer to the TaskRunner managing the task.
         */
        explicit Task(TaskRunner* runner) :
            _runner(runner),
            _status(std::make_shared<TaskStatus>(false, false)),
            _result()
        {
        }

        ~Task() override = default;

        [[nodiscard]] std::shared_ptr<const TaskStatus> getStatus() const override
        {
            return _status;
        }

        [[nodiscard]] bool hasFinished() const override
        {
            return _status->finished;
        }

        [[nodiscard]] bool isCancelled() const override
        {
            return _status->cancelled;
        }

        void wait() override
        {
            std::unique_lock lock(_valueMutex);
            if (_status->finished || _status->cancelled) {
                return;
            }
            _valueCondition.wait(lock);
        }

        void cancel() override
        {
            std::lock_guard lock(_valueMutex);
            if (_status->finished || _status->cancelled) {
                return;
            }
            _status->cancelled = true;
            _valueCondition.notify_all();
        }

        void addDependency(bool ignoreCancellations, const AbstractTask* dependency) override
        {
            _dependencies.emplace_back(ignoreCancellations, dependency->getStatus());
        }

        void provideContext(TaskRunner* runner) override
        {
            _runner = runner;
        }

        [[nodiscard]] bool isUnlocked() const override
        {
            return std::ranges::all_of(_dependencies, [](const auto& dependency) {
                return dependency.second->finished.load() || dependency.first && dependency.second->cancelled.load();
            });
        }

        [[nodiscard]] bool isAnyDependencyCancelled() const override
        {
            return std::ranges::any_of(_dependencies, [](const auto& dependency) {
                return !dependency.first && dependency.second->cancelled.load();
            });
        }

        /**
         * Sets the result of the task and marks it as finished.
         *
         * If the task has already finished or has been cancelled, the result is not assigned.
         *
         * @param result Rvalue reference to the result value.
         */
        void setResult(HoldValue&& result)
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
         * @brief Moves and returns the task's result.
         *
         * This function moves the stored result. It is not thread-safe; ensure that the task has finished.
         *
         * @return Rvalue of the task's result encapsulated in a std::optional.
         */
        std::optional<Result>&& moveResult()
            requires (!std::is_void_v<Result>)
        {
            return std::move(_result);
        }

        /**
         * Retrieves a reference to the task's result.
         *
         * This method is not thread-safe; it should only be used once task completion
         * has been verified.
         *
         * @return Reference to the optional result.
         */
        std::optional<Result>& getResult()
            requires (!std::is_void_v<Result>)
        {
            return _result;
        }

        /**
         * @brief Chains a function to be executed once this task completes.
         *
         * This overload schedules @p function to be executed after the task completes,
         * using default settings (do not ignore cancellations and execute asynchronously).
         *
         * @tparam Func Type of the function to chain.
         * @tparam Args Types of additional parameters for the function.
         * @param function The function to execute after completion.
         * @param args The arguments to forward to the function.
         * @return A shared pointer to a new Task monitoring the execution of the chained function.
         */
        template<typename Func, typename... Args>
        auto then(Func&& function, Args&&... args)
        {
            return then(false, false, std::forward<Func>(function), std::forward<Args>(args)...);
        }

        /**
         * @brief Chains a function to be executed once this task completes.
         *
         * Schedules the provided @p function to be executed after this task completes.
         *
         * @tparam Func Type of the function to chain.
         * @tparam Args Types of additional parameters.
         * @param ignoreCancellations If true, ignore cancellations of dependencies.
         * @param runOnMainThread If true, schedule the function to run on the main thread.
         * @param function The function to execute.
         * @param args Arguments to be passed to the function.
         * @return A shared pointer to a new Task that monitors the state of the chained execution.
         */
        template<typename Func, typename... Args>
        auto then(bool ignoreCancellations, bool runOnMainThread, Func&& function, Args&&... args)
            -> std::shared_ptr<Task<decltype(function(std::forward<Args>(args)...))>>;
    };

    /**
     * Structure used by a TaskRunner representing a scheduled running task.
     *
     * This structure encapsulates the task, the function to be executed,
     * and whether it should run on the main thread.
     */
    struct RunningTask
    {
        std::shared_ptr<AbstractTask> task;
        std::function<void()> function;
        bool runOnMainThread;
    };

    /**
     * Manages and executes user-defined tasks.
     *
     * The TaskRunner schedules tasks to be run either asynchronously or on the main thread.
     * Although instances of TaskRunner can be created, it is recommended to use the one available
     * at the Application level.
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
         * Shuts down the TaskRunner.
         *
         * Stops accepting new tasks and waits for all asynchronous tasks to finish.
         * Note that tasks scheduled on the main thread (including coroutines) are not awaited.
         */
        void shutdown();

        /**
         * Launches all tasks scheduled to run on the main thread.
         *
         * This includes tasks and stored coroutines. It is intended to be called
         * from the main thread.
         */
        void flushMainThreadTasks();

        /**
         * Stores and schedules a coroutine to be launched on the main thread.
         *
         * The coroutine is stored internally and will be executed the next time
         * flushMainThreadTasks() is called.
         *
         * @tparam Coroutine The coroutine type.
         * @param coroutine The coroutine instance to schedule.
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
         * Schedules a function to be executed on the main thread.
         *
         * Executes the provided function on the main thread with the given arguments.
         * The execution result (if any) is stored in the returned Task instance.
         *
         * @tparam Return The return type of the function.
         * @tparam FParams The parameter types of the function.
         * @param function The function to execute.
         * @param args Arguments passed to the function.
         * @return A shared pointer to a Task monitoring the function's execution.
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
                    task->setResult(std::monostate());
                } else {
                    task->setResult(function(std::move(std::get<std::decay_t<Args>>(tu))...));
                }
            };

            manageRunningTaskAddition(std::move(running));

            return task;
        }

        /**
         * Schedules a function to be executed on the main thread using an existing Task.
         *
         * The result of the execution is stored in the provided @p task.
         *
         * @tparam Return The return type of the function.
         * @tparam Func The type of the function.
         * @tparam Args The parameter types.
         * @param task The Task instance to manage the execution result.
         * @param function The function to execute.
         * @param args Arguments to pass to the function.
         */
        template<typename Return, typename Func, typename... Args>
        void executeOnMainThread(std::shared_ptr<Task<Return>> task, Func&& function, Args&&... args)
        {
            using Tu = std::tuple<std::decay_t<Args>...>;
            if (_stop) {
                return;
            }
            std::shared_ptr tuple = std::make_shared<Tu>(std::forward<Args>(args)...);

            RunningTask running{.task = task, .runOnMainThread = true};

            running.function = [task, fun = std::forward<Func>(function), t = std::move(tuple)] {
                if (task->isCancelled()) {
                    return;
                }
                Tu tu = std::move(*t);
                if constexpr (std::is_void_v<Return>) {
                    fun(std::move(std::get<std::decay_t<Args>>(tu))...);
                    task->setResult(std::monostate());
                } else {
                    task->setResult(fun(std::move(std::get<std::decay_t<Args>>(tu))...));
                }
            };

            manageRunningTaskAddition(std::move(running));
        }

        /**
         * @brief Schedules a function to be executed asynchronously.
         *
         * The provided function is executed on a worker thread. Its result
         * is stored in the Task returned.
         *
         * @tparam Func The type of the function.
         * @tparam Args The types of the function parameters.
         * @param function The function to be executed.
         * @param args Arguments to pass to the function.
         * @return A shared pointer to a Task monitoring the asynchronous execution.
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

            running.function = [task, fun = std::forward<Func>(function), t = std::move(tuple)] {
                if (task->isCancelled()) {
                    return;
                }
                Tu tu = std::move(*t);
                if constexpr (std::is_void_v<Return>) {
                    fun(std::move(std::get<std::decay_t<Args>>(tu))...);
                    task->setResult(std::monostate());
                } else {
                    task->setResult(fun(std::move(std::get<std::decay_t<Args>>(tu))...));
                }
            };

            manageRunningTaskAddition(std::move(running));

            return task;
        }

        /**
         * @brief Schedules a function to be executed asynchronously using an existing Task.
         *
         * The result of the function execution is stored in the provided @p task.
         *
         * @tparam Return The return type of the function.
         * @tparam Func The type of the function.
         * @tparam Args The types of the function parameters.
         * @param task The Task instance that will store the result.
         * @param function The function to execute.
         * @param args Arguments to pass to the function.
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

            running.function = [task, fun = std::forward<Func>(function), t = std::move(tuple)] {
                if (task->isCancelled()) {
                    return;
                }
                Tu tu = std::move(*t);
                if constexpr (std::is_void_v<Return>) {
                    fun(std::move(std::get<std::decay_t<Args>>(tu))...);
                    task->setResult(std::monostate());
                } else {
                    task->setResult(fun(std::move(std::get<std::decay_t<Args>>(tu))...));
                }
            };

            manageRunningTaskAddition(std::move(running));
        }
    };

    template<typename Result>
    template<typename Func, typename... Args>
    auto Task<Result>::then(bool ignoreCancellations, bool runOnMainThread, Func&& function, Args&&... args)
        -> std::shared_ptr<Task<decltype(function(std::forward<Args>(args)...))>>
    {
        using Return = decltype(function(std::forward<Args>(args)...));
        std::shared_ptr<Task<Return>> task = std::make_shared<Task<Return>>(_runner);
        task->addDependency(ignoreCancellations, this);

        if (_runner == nullptr) {
            error() << "This task has no context. If this is a coroutine's task, launch it before using this function.";
            return nullptr;
        }

        if (runOnMainThread) {
            _runner->executeOnMainThread(task, std::forward<Func>(function), std::forward<Args>(args)...);
        } else {
            _runner->executeAsync(task, function, std::forward<Args>(args)...);
        }
        return task;
    }
} // namespace neon

#endif // TASKRUNNER_H
