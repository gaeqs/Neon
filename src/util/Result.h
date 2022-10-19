//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_RESULT_H
#define RVTRACKING_RESULT_H

#include <optional>

template<class Ok, class Err>
class Result {
    std::optional<Ok> _ok;
    std::optional<Err> _err;

public:

    Result(const Ok& ok) : _ok(ok), _err() {
    }

    Result(const Err& err) : _ok(), _err(err) {
    }

    bool isOk() {
        return _ok.has_value();
    }

    Ok& getResult() {
        return _ok.value();
    }

    const Ok& getResult() const {
        return _ok.value();
    }

    Err& getError() {
        return _err.value();
    }

    const Err& getError() const {
        return _err.value();
    }
};

#endif //RVTRACKING_RESULT_H
