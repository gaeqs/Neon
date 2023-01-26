//
// Created by gaelr on 22/01/2023.
//

#ifndef NEON_PROFILESTACKRECORDER_H
#define NEON_PROFILESTACKRECORDER_H

#include <memory>
#include <chrono>

class Profiler;

class ProfileStack;

class ProfileStackRecorder {

    Profiler* _profiler;
    std::shared_ptr<ProfileStack> _stack;
    std::chrono::high_resolution_clock::time_point _start;

public:

    ProfileStackRecorder(const ProfileStackRecorder& other) = delete;

    ProfileStackRecorder(Profiler* profiler,
                         std::shared_ptr<ProfileStack> stack);

    ~ProfileStackRecorder();

};


#endif //NEON_PROFILESTACKRECORDER_H
