//
// Created by gaeqs on 16/11/25.
//

#ifndef NEON_HSLCOLORSPACE_H
#define NEON_HSLCOLORSPACE_H

#include <rush/vector/vec.h>

namespace neon
{

    template<bool Alpha = false>
    struct HSLColorSpace
    {
        static constexpr size_t EntriesAmount = Alpha ? 4 : 3;
        using Entry = float;
        using Data = rush::Vec<EntriesAmount, float>;

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

        static Data lighter(const Data& data, const Entry& amount)
        {
            Data modified = data;
            modified[2] = std::clamp(modified[2] + amount, static_cast<Entry>(0), static_cast<Entry>(1));
            return modified;
        }

        static Data darker(const Data& data, const Entry& amount)
        {
            Data modified = data;
            modified[2] = std::clamp(modified[2] - amount, static_cast<Entry>(0), static_cast<Entry>(1));
            return modified;
        }

        static Data rotateHue(const Data& data, Entry amount)
        {
            Data modified = data;
            modified[0] = std::fmod(modified[0] + amount, static_cast<Entry>(1));
            if (modified[0] < static_cast<Entry>(0)) {
                modified[0] += static_cast<Entry>(1);
            }
            return modified;
        }

        static Data complementary(const Data& data)
        {
            return rotateHue(data, static_cast<Entry>(0.5));
        }

        static Data triadic(const Data& data)
        {
            return rotateHue(data, static_cast<Entry>(1.0 / 3.0));
        }

        static void print(std::ostream& os, const Data& data)
        {
            if constexpr (Alpha) {
                os << "HSLA (";
            } else {
                os << "HSL (";
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

    using HSLAColorSpace = HSLColorSpace<true>;

} // namespace neon

#endif // NEON_HSLCOLORSPACE_H
