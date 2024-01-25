//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_RESULT_H
#define RVTRACKING_RESULT_H

#include <functional>

namespace neon {
    /**
     * Represents the result of an operation.
     * This struct may contain the result data of the
     * operation or an error informing why the operation
     * has failed.
     * @tparam Ok the type of the result.
     * @tparam Error the error type.
     */
    template<class Ok, class Error>
    class Result {
        Ok _ok;
        Error _error;
        bool _valid;

    public:
        Result(const Ok& ok) : _ok(ok), _error(), _valid(true) {
        }

        Result(Ok&& ok) : _ok(std::move(ok)), _error(), _valid(true) {
        }

        Result(const Error& error) : _ok(), _error(error), _valid(false) {
        }

        Result(Error&& error) : _ok(), _error(std::move(error)), _valid(false) {
        }

        [[nodiscard]] bool isOk() const {
            return _valid;
        }

        [[nodiscard]] Ok& getResult() {
            return _ok;
        }

        [[nodiscard]] const Ok& getResult() const {
            return _ok;
        }

        [[nodiscard]] Error& getError() {
            return _error;
        }

        [[nodiscard]] const Error& getError() const {
            return _error;
        }

        [[nodiscard]] Ok& orElse(Ok& other) {
            return _valid ? _ok : other;
        }

        [[nodiscard]] const Ok& orElse(Ok& other) const {
            return _valid ? _ok : other;
        }

        [[nodiscard]] Ok orElseGet(std::function<Ok()> provider) const {
            return _valid ? _ok : provider();
        }

        template<class OOk>
        [[nodiscard]] Result<OOk, Error>
        map(std::function<OOk(Ok)> mapper) const {
            // Don't use a ternary operator.
            // The return value is implicitly transformed
            // into a Result using different constructors.
            // Using a ternary operator disallows that.
            if (_valid) return mapper(_ok);
            return _error;
        }

        template<class OError>
        [[nodiscard]] Result<OError, Error>
        mapError(std::function<OError(Error)> mapper) const {
            // Don't use a ternary operator.
            // The return value is implicitly transformed
            // into a Result using different constructors.
            // Using a ternary operator Idisallows that.
            if (_valid) return _ok;
            return mapper(_error);
        }
    };
}

#endif //RVTRACKING_RESULT_H
