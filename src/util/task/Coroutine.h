//
// Created by gaeqs on 8/21/24.
//

#ifndef COROUTINE_H
#define COROUTINE_H
#include <chrono>
#include <coroutine>
#include <exception>
#include <functional>
#include <memory>

namespace neon {
    class CoroutineWaitReason {
    public:
        virtual ~CoroutineWaitReason() = default;

        virtual bool isReady() = 0;
    };

    struct Coroutine {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        struct promise_type // required
        {
            std::unique_ptr<CoroutineWaitReason> _currentWaitReason;
            std::exception_ptr _exception;

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

            std::suspend_always yield_value(CoroutineWaitReason* reason) {
                _currentWaitReason = std::unique_ptr<CoroutineWaitReason>(
                    reason);
                return {};
            }

            void return_void() {}
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

        ~Coroutine() {
            if(_valid) {
                _handler.destroy();
            }
        }

        [[nodiscard]] bool isReady() const {
            if(isDone()) return false;
            auto& reason = _handler.promise()._currentWaitReason;
            return reason == nullptr || reason->isReady();
        }

        [[nodiscard]] bool isDone() const {
            return !_valid || _handler.done();
        }

        void launch() const {
            if(!_valid || _handler.done()) return;
            _handler();
        }

        Coroutine& operator=(Coroutine&& other) {
            if(_valid) {
                _handler.destroy();
            }

            _handler = other._handler;
            _valid = other._valid;
            other._valid = false;
            return *this;
        }
    };


    class WaitForSeconds : public CoroutineWaitReason {
        std::chrono::system_clock::time_point _wakeUpTime;

    public:
        explicit WaitForSeconds(float seconds);

        ~WaitForSeconds() override = default;

        bool isReady() override;
    };

    class WaitForNextFrame : public CoroutineWaitReason {
    public:
        explicit WaitForNextFrame();

        ~WaitForNextFrame() override = default;

        bool isReady() override;
    };

    class WaitUntil : public CoroutineWaitReason {
        std::function<bool()> _predicate;

    public:
        explicit WaitUntil(std::function<bool()> predicate);

        ~WaitUntil() override = default;

        bool isReady() override;
    };

    class WaitWhile : public CoroutineWaitReason {
        std::function<bool()> _predicate;

    public:
        explicit WaitWhile(std::function<bool()> predicate);

        ~WaitWhile() override = default;

        bool isReady() override;
    };
}

#endif //COROUTINE_H
