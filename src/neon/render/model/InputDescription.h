//
// Created by grial on 22/11/22.
//

#ifndef NEON_INPUTDESCRIPTION_H
#define NEON_INPUTDESCRIPTION_H

#include <cstdint>
#include <vector>

namespace neon
{
    enum class InputRate
    {
        VERTEX,
        INSTANCE
    };

    struct InputAttribute
    {
        uint32_t sizeInFloats;
        uint32_t offsetInBytes;

        InputAttribute(uint32_t sizeInFloats_, uint32_t offsetInBytes_) :
            sizeInFloats(sizeInFloats_),
            offsetInBytes(offsetInBytes_)
        {
        }
    };

    struct InputDescription
    {
        uint32_t stride;
        InputRate rate;
        std::vector<InputAttribute> attributes;

        InputDescription(uint32_t stride_, InputRate rate_) :
            stride(stride_),
            rate(rate_),
            attributes()
        {
        }

        void addAttribute(uint32_t sizeInFloats, uint32_t offsetInBytes)
        {
            attributes.emplace_back(sizeInFloats, offsetInBytes);
        }
    };
} // namespace neon

#endif //NEON_INPUTDESCRIPTION_H
