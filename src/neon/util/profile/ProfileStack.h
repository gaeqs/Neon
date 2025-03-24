//
// Created by gaelr on 22/01/2023.
//

#ifndef NEON_PROFILESTACK_H
#define NEON_PROFILESTACK_H

#include <chrono>
#include <optional>
#include <deque>
#include <unordered_map>
#include <memory>
#include <string>

namespace neon
{
    class ProfileStack
    {
        using ProfileDuration = std::chrono::duration<uint64_t, std::nano>;

        std::string _name;
        ProfileStack* _parent;
        std::deque<ProfileDuration> _durations;
        std::unordered_map<std::string, std::shared_ptr<ProfileStack>> _children;

      public:
        constexpr static int MAX_DURATIONS = 10;

        ProfileStack(std::string name, ProfileStack* parent);

        void registerDuration(ProfileDuration duration);

        [[nodiscard]] const std::string& getName() const;

        [[nodiscard]] ProfileStack* getParent() const;

        [[nodiscard]]
        const std::unordered_map<std::string, std::shared_ptr<ProfileStack>>& getChildren() const;

        [[nodiscard]] std::optional<std::shared_ptr<ProfileStack>> getChild(const std::string& name) const;

        [[nodiscard]] std::shared_ptr<ProfileStack> getOrCreateChild(const std::string& name);

        [[nodiscard]] const std::deque<ProfileDuration>& getDurations() const;

        [[nodiscard]] ProfileDuration getAverageDuration() const;
    };
} // namespace neon

#endif //NEON_PROFILESTACK_H
