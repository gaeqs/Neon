//
// Created by gaeqs on 16/11/25.
//

#ifndef NEON_RGBCOLORSPACE_H
#define NEON_RGBCOLORSPACE_H

#include <rush/vector/vec.h>

namespace neon
{
    template<bool Alpha = false>
    struct RGBColorSpace
    {
        static constexpr size_t EntriesAmount = Alpha ? 4 : 3;
        using Entry = float;
        using Data = rush::Vec<EntriesAmount, float>;

        template<typename... T>
            requires(std::conjunction_v<std::is_integral<T>...> && sizeof...(T) == EntriesAmount)
        static Data fromUInts(T... list)
        {
            if constexpr (Alpha) {
                return rush::Vec4ui(list...).cast<Entry>() / static_cast<Entry>(255);
            } else {
                return rush::Vec3ui(list...).cast<Entry>() / static_cast<Entry>(255);
            }
        }

        template<typename... T>
            requires(std::conjunction_v<std::is_floating_point<T>...> && sizeof...(T) == EntriesAmount)
        static Data fromFloats(T... list)
        {
            if constexpr (Alpha) {
                return rush::Vec4f(list...).cast<Entry>();
            } else {
                return rush::Vec3f(list...).cast<Entry>();
            }
        }

        static Data fromUInt32(const uint32_t color)
        {
            // This is from ARGB, not RGBA!
            // Usually, colors defined using vectors use RGBA, while colors defined using an uint32_t use ARGB!
            uint32_t r = color >> 16 & 0xFF;
            uint32_t g = color >> 8 & 0xFF;
            uint32_t b = color & 0xFF;
            if constexpr (Alpha) {
                uint32_t a = color >> 24 & 0xFF;
                return rush::Vec4ui(r, g, b, a).cast<Entry>() / static_cast<Entry>(255);
            } else {
                return rush::Vec3ui(r, g, b).cast<Entry>() / static_cast<Entry>(255);
            }
        }

        static uint32_t toUInt(const Data& data)
        {
            constexpr Entry MAX = static_cast<Entry>(255);
            if constexpr (Alpha) {
                return static_cast<uint32_t>(data[0] * MAX) << 16 | static_cast<uint32_t>(data[1] * MAX) << 8 |
                       static_cast<uint32_t>(data[2] * MAX) | static_cast<uint32_t>(data[3] * MAX) << 24;
            } else {
                return static_cast<uint32_t>(data[0] * MAX) << 16 | static_cast<uint32_t>(data[1] * MAX) << 8 |
                       static_cast<uint32_t>(data[2] * MAX) | 0xFF000000;
            }
        }

        static void print(std::ostream& os, const Data& data)
        {
            if constexpr (Alpha) {
                os << "RGBA (";
            } else {
                os << "RGB (";
            }
            for (size_t i = 0; i < data.size(); ++i) {
                os << data[i];
                if (i < data.size() - 1) {
                    os << ", ";
                }
            }
            os << ")";
        }
    };

    using RGBAColorSpace = RGBColorSpace<true>;

} // namespace neon

#endif // NEON_RGBCOLORSPACE_H
