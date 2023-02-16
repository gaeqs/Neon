//
// Created by gaelr on 22/01/2023.
//

#ifndef NEON_PROFILER_H
#define NEON_PROFILER_H

#include <unordered_map>
#include <thread>
#include <mutex>
#include <memory>
#include <string>

#include <util/profile/ProfileStack.h>
#include <util/profile/ProfileStackRecorder.h>

#ifdef NDEBUG
#define DEBUG_PROFILE(profiler,name)
#define DEBUG_PROFILE_ID(profiler, id, name)
#define DEBUG_PROFILE_PTR(profiler,name)
#define DEBUG_PROFILE_ID_PTR(profiler,name)
#else
#define DEBUG_PROFILE(profiler, name) auto name##ProfileStack = (profiler).push(#name)
#define DEBUG_PROFILE_ID(profiler, id, name) auto id##ProfileStack = (profiler).push(name)
#define DEBUG_PROFILE_PTR(profiler, name) auto name##ProfileStack = (profiler)->push(#name)
#define DEBUG_PROFILE_ID_PTR(profiler, id, name) auto id##ProfileStack = (profiler)->push(name)
#endif

namespace neon {

    class Profiler {

        std::unordered_map<std::thread::id, std::unique_ptr<ProfileStack>> _roots;
        std::unordered_map<std::thread::id, ProfileStack*> _current;
        std::mutex _mutex;

    public:

        Profiler();

        std::unordered_map<std::thread::id, ProfileStack*> getProfiles();

        std::lock_guard<std::mutex> lockProfiles();

        ProfileStackRecorder push(const std::string& name);

        void pop();

    };
}

#endif //NEON_PROFILER_H
