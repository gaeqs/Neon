//
// Created by gaeqs on 16/01/2025.
//

#include "Chronometer.h"

namespace neon {
    Chronometer::Chronometer()
        : _start(std::chrono::high_resolution_clock::now()) {}

    std::chrono::nanoseconds Chronometer::elapsed() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now - _start);
    }

    size_t Chronometer::elapsedNanoseconds() const {
        return static_cast<size_t>(elapsed().count());
    }

    float Chronometer::elapsedMilliseconds() const {
        return static_cast<float>(elapsedNanoseconds()) / 1.0e6f;
    }

    float Chronometer::elapsedSeconds() const {
        return static_cast<float>(elapsedNanoseconds()) / 1.0e9f;
    }

    void Chronometer::reset() {
        _start = std::chrono::high_resolution_clock::now();
    }
}
