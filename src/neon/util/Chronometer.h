//
// Created by gaeqs on 16/01/2025.
//

#ifndef CHRONOMETER_H
#define CHRONOMETER_H
#include <chrono>

namespace neon {
    class Chronometer {
        std::chrono::high_resolution_clock::time_point _start;

    public:
        Chronometer();

        [[nodiscard]] std::chrono::nanoseconds elapsed() const;

        [[nodiscard]] size_t elapsedNanoseconds() const;

        [[nodiscard]] float elapsedMilliseconds() const;

        [[nodiscard]] float elapsedSeconds() const;

        void reset();
    };
}


#endif //CHRONOMETER_H
