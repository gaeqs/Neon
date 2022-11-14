//
// Created by gaelr on 10/11/2022.
//

#ifndef NEON_VKQUEUEFAMILYINDICES_H
#define NEON_VKQUEUEFAMILYINDICES_H

#include <cstdint>
#include <optional>
#include <unordered_set>

struct VKQueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;

    [[nodiscard]] std::unordered_set<uint32_t> getValidIndices() const {
        std::unordered_set<uint32_t> set;
        if (graphics.has_value()) {
            set.insert(graphics.value());
        }
        if (present.has_value()) {
            set.insert(present.value());
        }
        return set;
    }

    [[nodiscard]] bool isComplete() const {
        return graphics.has_value() && present.has_value();
    }
};

#endif //NEON_VKQUEUEFAMILYINDICES_H
