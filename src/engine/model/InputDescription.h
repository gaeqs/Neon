//
// Created by grial on 22/11/22.
//

#ifndef NEON_INPUTDESCRIPTION_H
#define NEON_INPUTDESCRIPTION_H

#include <cstdint>
#include <vector>

enum class InputRate {
    VERTEX,
    INSTANCE
};

struct InputAttribute {
    uint32_t sizeInFloats;
    uint32_t offsetInBytes;

    InputAttribute(uint32_t sizeInFloats, uint32_t offsetInBytes)
            : sizeInFloats(sizeInFloats), offsetInBytes(offsetInBytes) {}
};

struct InputDescription {
    uint32_t stride;
    InputRate rate;
    std::vector<InputAttribute> attributes;

    InputDescription(uint32_t stride, InputRate rate)
            : stride(stride),
              rate(rate),
              attributes() {
    }

    void addAttribute(uint32_t sizeInFloats, uint32_t offsetInBytes) {
        attributes.emplace_back(sizeInFloats, offsetInBytes);
    }
};


#endif //NEON_INPUTDESCRIPTION_H
