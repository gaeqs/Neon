//
// Created by gaeqs on 8/21/24.
//

#ifndef COROUTINE_H
#define COROUTINE_H
#include <chrono>
#include <coroutine>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>

#include <util/task/AbstractCoroutine.h>
#include <util/task/TaskRunner.h>

namespace neon {
    /**
     * Base class use to create coroutine pauses.
     * When a coroutine co_yields an extension of this class,
     * the coroutine will suspend until isReady returns true.
     */
    class CoroutineWaitReason {
    public:
        /**
         * Deletes this wait reason.
         */
        virtual ~CoroutineWaitReason() = default;

        /**
         * @return whether the coroutine is ready to be launched again.
         */
        virtual bool isReady() = 0;
    };

    /**
     * A coroutine implementation that may have a result value.
     * <p>
     * To use this struct, create a function that returns neon::Coroutine.
     * Then, use the keyword co_yield to add breaks to the coroutine.
     * Once all the coroutine's job has finished, use co_return to return
     * the result of the coroutine.
     * <p>
     * Use co_yield with a class that extends CoroutineWaitReason to
     * make your coroutine wait for a specific condition.
     * <p>
     * You may use the TaskRunner instance inside Application
     * to launch this coroutine.
     *
     * @tparam Return the type of the return value.
     */
    template<typename Return = void>
    struct Coroutine : AbstractCoroutine {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        struct promise_type // required
        {
            std::unique_ptr<CoroutineWaitReason> _currentWaitReason;
            std::exception_ptr _exception;

            std::shared_ptr<Task<Return>> _task;

            promise_type() : _task(std::make_shared<Task<Return>>()) {}

            Coroutine get_return_object() {
                return {handle_type::from_promise(*this)};
            }

            std::suspend_always initial_suspend() {
                return {};
            }

            std::suspend_always final_suspend() noexcept {
                return {};
            }

            void unhandled_exception() {
                _exception = std::current_exception();
            }

            template<typename T>
            std::suspend_always yield_value(const T& reason) {
                _currentWaitReason = std::make_unique<T>(reason);
                return {};
            }

            void return_value(auto value) {
                std::cout << typeid(decltype(value)).name() << std::endl;
                if constexpr(!std::is_void_v<Return>) {
                    _task->setResult(std::move(value));
                }
            }
        };

        handle_type _handler;
        bool _valid;

        Coroutine(handle_type h) : _handler(h), _valid(true) {}

        Coroutine(const Coroutine& other) = delete;

        Coroutine(Coroutine&& other) noexcept :
            _handler(other._handler),
            _valid(other._valid) {
            other._valid = false;
        }

        ~Coroutine() override {
            if(_valid) {
                _handler.destroy();
            }
        }

        [[nodiscard]] bool isReady() const override {
            if(isDone()) return false;
            auto& reason = _handler.promise()._currentWaitReason;
            return reason == nullptr || reason->isReady();
        }

        [[nodiscard]] bool isDone() const override {
            return !_valid || _handler.done();
        }

        [[nodiscard]] bool isValid() const override {
            return _valid;
        }

        void launch() const override {
            if(!_valid || _handler.done()) return;
            _handler();
        }

        /**
         * Returns the neon::Task representing this coroutine.
         * Use this task instance to check the status of this
         * coroutine.
         * <p>
         * You should use this task instance to retrieve the
         * result of the coroutine.
         * @return the neon::Task representing this coroutine.
         */
        const std::shared_ptr<Task<Return>>& asTask() {
            return _handler.promise()._task;
        }

        Coroutine& operator=(Coroutine&& other) noexcept {
            if(_valid) {
                _handler.destroy();
            }

            _handler = other._handler;
            _valid = other._valid;
            other._valid = false;
            return *this;
        }
    };

    /**
     * A coroutine implementation that doesn't have a result value.
     * <p>
     * To use this struct, create a function that returns neon::Coroutine.
     * Then, use the keyword co_yield to add breaks to the coroutine.
     * Once all the coroutine's job has finished, you can use co_return.
     * <p>
     * Use co_yield with a class that extends CoroutineWaitReason to
     * make your coroutine wait for a specific condition.
     * <p>
     * You may use the TaskRunner instance inside Application
     * to launch this coroutine.
     *
     */
    template<>
    struct Coroutine<void> : AbstractCoroutine {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        struct promise_type // required
        {
            std::unique_ptr<CoroutineWaitReason> _currentWaitReason;
            std::exception_ptr _exception;

            std::shared_ptr<Task<void>> _task;

            promise_type() : _task(std::make_shared<Task<void>>()) {}

            Coroutine get_return_object() {
                return {handle_type::from_promise(*this)};
            }

            std::suspend_always initial_suspend() {
                return {};
            }

            std::suspend_always final_suspend() noexcept {
                return {};
            }

            void unhandled_exception() {
                _exception = std::current_exception();
            }

            template<typename T>
            std::suspend_always yield_value(const T& reason) {
                _currentWaitReason = std::make_unique<T>(reason);
                return {};
            }

            void return_void() {
                _task->finish();
            }
        };

        handle_type _handler;
        bool _valid;

        Coroutine(handle_type h) : _handler(h), _valid(true) {}

        Coroutine(const Coroutine& other) = delete;

        Coroutine(Coroutine&& other) noexcept :
            _handler(other._handler),
            _valid(other._valid) {
            other._valid = false;
        }

        ~Coroutine() override {
            if(_valid) {
                _handler.destroy();
            }
        }

        [[nodiscard]] bool isReady() const override {
            if(isDone()) return false;
            auto& reason = _handler.promise()._currentWaitReason;
            return reason == nullptr || reason->isReady();
        }

        [[nodiscard]] bool isDone() const override {
            return !_valid || _handler.done();
        }

        [[nodiscard]] bool isValid() const override {
            return _valid;
        }

        void launch() const override {
            if(!_valid || _handler.done()) return;
            _handler();
        }

        /**
         * Returns the neon::Task representing this coroutine.
         * Use this task instance to check the status of this
         * coroutine.
         *
         * @return the neon::Task representing this coroutine.
         */
        const std::shared_ptr<Task<void>>& asTask() {
            return _handler.promise()._task;
        }

        Coroutine& operator=(Coroutine&& other) noexcept {
            if(_valid) {
                _handler.destroy();
            }

            _handler = other._handler;
            _valid = other._valid;
            other._valid = false;
            return *this;
        }
    };

    /**
     * Suspends the coroutine for the given amount of seconds.
     */
    class WaitForSeconds : public CoroutineWaitReason {
        std::chrono::system_clock::time_point _wakeUpTime;

    public:
        /**
         * Suspends the coroutine for the given amount of seconds.
         */
        explicit WaitForSeconds(float seconds);

        ~WaitForSeconds() override = default;

        bool isReady() override;
    };

    /**
     * Suspends the coroutine and resumes it the next frame
     */
    class WaitForNextFrame : public CoroutineWaitReason {
    public:

        /**
         * Suspends the coroutine and resumes it the next frame
         */
        explicit WaitForNextFrame();

        ~WaitForNextFrame() override = default;

        bool isReady() override;
    };

    /**
     * Suspends the coroutine until the given predicate returns true.
     */
    class WaitUntil : public CoroutineWaitReason {
        std::function<bool()> _predicate;

    public:

        /**
         * Suspends the coroutine until the given predicate returns true.
         */
        explicit WaitUntil(std::function<bool()> predicate);

        ~WaitUntil() override = default;

        bool isReady() override;
    };

    /**
     * Suspends the coroutine until the given predicate returns false.
     */
    class WaitWhile : public CoroutineWaitReason {
        std::function<bool()> _predicate;

    public:

        /**
         * Suspends the coroutine until the given predicate returns false.
         */
        explicit WaitWhile(std::function<bool()> predicate);

        ~WaitWhile() override = default;

        bool isReady() override;
    };
}

#endif //COROUTINE_H
