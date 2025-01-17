//
// Created by gaeqs on 16/01/2025.
//

#ifndef CHRONOMETER_H
#define CHRONOMETER_H
#include <chrono>

namespace neon {
    /**
    * Small util class used to measure durations.
    * <p>
    * The start time is the time point the instance is created.
    * You can reset the start time using the method 'reset()'.
    */
    class Chronometer {
        std::chrono::high_resolution_clock::time_point _start;

    public:
        /**
        * Creates a chronometer.
        * The count start at the creation of the instance.
        */
        Chronometer();

        /**
        * Returns the time elapsed since the start time.
        */
        [[nodiscard]] std::chrono::nanoseconds elapsed() const;

        /**
        * Returns the time elapsed since the start time in nanoseconds.
        */
        [[nodiscard]] size_t elapsedNanoseconds() const;

        /**
        * Returns the time elapsed since the start time in milliseconds.
        */
        [[nodiscard]] float elapsedMilliseconds() const;

        /**
        * Returns the time elapsed since the start time in seconds.
        */
        [[nodiscard]] float elapsedSeconds() const;

        /**
        * Resets the start time.
        */
        void reset();
    };
}


#endif //CHRONOMETER_H
