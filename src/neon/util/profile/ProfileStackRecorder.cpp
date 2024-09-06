//
// Created by gaelr on 22/01/2023.
//

#include "ProfileStackRecorder.h"

#include <utility>

#include <neon/util/profile/Profiler.h>
#include <neon/util/profile/ProfileStack.h>

namespace neon {
    ProfileStackRecorder::ProfileStackRecorder(
            Profiler* profiler,
            std::shared_ptr<ProfileStack> stack) :
            _profiler(profiler),
            _stack(std::move(stack)),
            _start(std::chrono::high_resolution_clock::now()) {
    }

    ProfileStackRecorder::~ProfileStackRecorder() {
        auto duration = std::chrono::high_resolution_clock::now() - _start;
        _stack->registerDuration(duration);
        _profiler->pop();
    }
}