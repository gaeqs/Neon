//
// Created by gaeqs on 3/12/24.
//

#include <neon/structure/Identifiable.h>

namespace neon {
    Identifiable::Identifiable()
        : _counter(new IdentifiableCounter(0, true, this)) {}

    Identifiable::Identifiable(const Identifiable&)
        : _counter(new IdentifiableCounter(0, true, this)) {
        // On copy, create a new value.
    }

    Identifiable::Identifiable(Identifiable&& other) noexcept
        : _counter(other._counter) {
        if (_counter != nullptr) {
            _counter->ptr = this;
        }
        other._counter = nullptr;
    }

    Identifiable::~Identifiable() {
        if (_counter != nullptr) {
            _counter->valid = false;
            _counter->ptr = nullptr;
            if (_counter->counter == 0) {
                delete _counter;
            }
        }
    }

    IdentifiableCounter* Identifiable::getCounter() const {
        return _counter;
    }
}
