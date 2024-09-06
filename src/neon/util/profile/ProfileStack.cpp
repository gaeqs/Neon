//
// Created by gaelr on 22/01/2023.
//

#include "ProfileStack.h"

#include <utility>

namespace neon {
    ProfileStack::ProfileStack(std::string name, ProfileStack* parent) :
            _name(std::move(name)),
            _parent(parent),
            _durations(),
            _children() {

    }

    void ProfileStack::registerDuration(ProfileDuration duration) {
        _durations.push_back(duration);
        if (_durations.size() > MAX_DURATIONS) {
            _durations.pop_front();
        }
    }

    std::optional<std::shared_ptr<ProfileStack>>
    ProfileStack::getChild(const std::string& name) const {
        auto it = _children.find(name);
        if (it == _children.end()) return {};
        return it->second;
    }

    std::shared_ptr<ProfileStack>
    ProfileStack::getOrCreateChild(const std::string& name) {
        auto it = _children.find(name);
        if (it == _children.end()) {
            auto ptr = std::make_shared<ProfileStack>(name, this);
            _children[name] = ptr;
            return ptr;
        }
        return it->second;
    }

    const std::deque<ProfileStack::ProfileDuration>&
    ProfileStack::getDurations() const {
        return _durations;
    }

    ProfileStack::ProfileDuration ProfileStack::getAverageDuration() const {
        if (_durations.empty()) return ProfileDuration(0);
        ProfileDuration duration{0};
        for (const auto& d: _durations) {
            duration += d;
        }
        return duration / _durations.size();
    }

    const std::string& ProfileStack::getName() const {
        return _name;
    }

    ProfileStack* ProfileStack::getParent() const {
        return _parent;
    }

    const std::unordered_map<std::string, std::shared_ptr<ProfileStack>>&
    ProfileStack::getChildren() const {
        return _children;
    }
}