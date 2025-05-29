//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_RESULT_H
#define RVTRACKING_RESULT_H

#include <cstdint>
#include <functional>

namespace neon
{
    /**
     * Represents the result of an operation.
     * This struct may contain the result data of the
     * operation or an error informing why the operation
     * has failed.
     * @tparam Ok the type of the result.
     * @tparam Error the error type.
     */
    template<class Ok, class Error>
    class Result
    {
        void* _data;
        bool _valid;

      public:
        Result(const Ok& ok) :
            _valid(true)
        {
            _data = new uint8_t[sizeof(Ok)];
            std::construct_at<Ok, const Ok&>(static_cast<Ok*>(_data), ok);
        }

        Result(Ok&& ok) :
            _valid(true)
        {
            _data = new uint8_t[sizeof(Ok)];
            std::construct_at<Ok, Ok&&>(static_cast<Ok*>(_data), std::forward<Ok>(ok));
        }

        Result(const Error& error) :
            _valid(false)
        {
            _data = new uint8_t[sizeof(Error)];
            std::construct_at<Error, const Error&>(static_cast<Error*>(_data), error);
        }

        Result(Error&& error) :
            _valid(false)
        {
            _data = new uint8_t[sizeof(Error)];
            std::construct_at<Error, Error&&>(static_cast<Error*>(_data), std::forward<Error>(error));
        }

        ~Result()
        {
            if (_valid) {
                std::destroy_at<Ok>(static_cast<Ok*>(_data));
            } else {
                std::destroy_at<Error>(static_cast<Error*>(_data));
            }
            delete[] static_cast<uint8_t*>(_data);
        }

        [[nodiscard]] bool isOk() const
        {
            return _valid;
        }

        [[nodiscard]] Ok& getResult()
        {
            return *static_cast<Ok*>(_data);
        }

        [[nodiscard]] const Ok& getResult() const
        {
            return *static_cast<Ok*>(_data);
        }

        [[nodiscard]] Error& getError()
        {
            return *static_cast<Error*>(_data);
        }

        [[nodiscard]] const Error& getError() const
        {
            return *static_cast<Error*>(_data);
        }

        [[nodiscard]] Ok& orElse(Ok& other)
        {
            return _valid ? getResult() : other;
        }

        [[nodiscard]] const Ok& orElse(Ok& other) const
        {
            return _valid ? getResult() : other;
        }

        [[nodiscard]] Ok orElseGet(std::function<Ok()> provider) const
        {
            return _valid ? getResult() : provider();
        }

        template<class OOk>
        [[nodiscard]] Result<OOk, Error> map(std::function<OOk(Ok)> mapper) const
        {
            // Don't use a ternary operator.
            // The return value is implicitly transformed
            // into a Result using different constructors.
            // Using a ternary operator disallows that.
            if (_valid) {
                return mapper(getResult());
            }
            return getError();
        }

        template<class OError>
        [[nodiscard]] Result<OError, Error> mapError(std::function<OError(Error)> mapper) const
        {
            // Don't use a ternary operator.
            // The return value is implicitly transformed
            // into a Result using different constructors.
            // Using a ternary operator Idisallows that.
            if (_valid) {
                return getResult();
            }
            return mapper(getError());
        }
    };

    template<class Error>
    class Result<void, Error>
    {
        void* _data;
        bool _valid;

      public:
        Result() :
            _valid(true)
        {
            _data = nullptr;
        }

        Result(const Error& error) :
            _valid(false)
        {
            _data = new uint8_t[sizeof(Error)];
            std::construct_at<Error, const Error&>(static_cast<Error*>(_data), error);
        }

        Result(Error&& error) :
            _valid(false)
        {
            _data = new uint8_t[sizeof(Error)];
            std::construct_at<Error, Error&&>(static_cast<Error*>(_data), std::forward<Error>(error));
        }

        ~Result()
        {
            if (!_valid)  {
                std::destroy_at<Error>(static_cast<Error*>(_data));
            }
            delete[] static_cast<uint8_t*>(_data);
        }

        [[nodiscard]] bool isOk() const
        {
            return _valid;
        }

        [[nodiscard]] Error& getError()
        {
            return *static_cast<Error*>(_data);
        }

        [[nodiscard]] const Error& getError() const
        {
            return *static_cast<Error*>(_data);
        }

        template<class OError>
        [[nodiscard]] Result<void, OError> mapError(std::function<OError(Error)> mapper) const
        {
            // Don't use a ternary operator.
            // The return value is implicitly transformed
            // into a Result using different constructors.
            // Using a ternary operator Idisallows that.
            if (_valid) {
                return Result();
            }
            return mapper(getError());
        }
    };
} // namespace neon

#endif // RVTRACKING_RESULT_H
