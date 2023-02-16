//
// Created by gaelr on 22/01/2023.
//

#include "Profiler.h"

namespace neon {
    Profiler::Profiler() :
            _roots(),
            _current(),
            _mutex() {
    }

    ProfileStackRecorder Profiler::push(const std::string& name) {
        auto threadId = std::this_thread::get_id();
        std::shared_ptr<ProfileStack> child;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            auto current = _current.find(threadId);
            if (current == _current.end()) {
                // Root not found!
                auto root = std::make_unique<ProfileStack>("root", nullptr);
                child = root->getOrCreateChild(name);
                _roots[threadId] = std::move(root);
            } else {
                child = current->second->getOrCreateChild(name);
            }
            _current[threadId] = child.get();
        }

        return {this, child};
    }

    void Profiler::pop() {
        auto threadId = std::this_thread::get_id();
        {
            std::lock_guard<std::mutex> lock(_mutex);
            auto current = _current.find(threadId);
            if (current == _current.end() ||
                current->second->getParent() == nullptr) {
                throw std::runtime_error("Cannot pop root element!");
            }
            _current[threadId] = current->second->getParent();
        }
    }

    std::unordered_map<std::thread::id, ProfileStack*> Profiler::getProfiles() {
        std::unordered_map<std::thread::id, ProfileStack*> profiles;
        profiles.reserve(_roots.size());
        for (const auto& [id, profile]: _roots) {
            profiles[id] = profile.get();
        }
        return profiles;
    }

    std::lock_guard<std::mutex> Profiler::lockProfiles() {
        return std::lock_guard<std::mutex>(_mutex);
    }
}