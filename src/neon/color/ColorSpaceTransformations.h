//
// Created by gaeqs on 16/11/25.
//

#ifndef NEON_COLORSPACETRANSFORMATIONS_H
#define NEON_COLORSPACETRANSFORMATIONS_H

#include <neon/color/RGBColorSpace.h>
#include <neon/color/HSLColorSpace.h>
#include <neon/color/HSVColorSpace.h>

namespace neon
{

    // Wrapper
    template<typename FromCS, typename ToCS>
    struct ColorTransformer;

    template<bool FromAlpha, bool ToAlpha>
    struct ColorTransformer<RGBColorSpace<FromAlpha>, RGBColorSpace<ToAlpha>>
    {
        using Data = RGBColorSpace<FromAlpha>::Data;
        using ToData = RGBColorSpace<ToAlpha>::Data;
        using Entry = RGBColorSpace<FromAlpha>::Entry;
        using ToEntry = RGBColorSpace<ToAlpha>::Entry;

        static ToData transform(const Data& rgb)
        {
            if constexpr (ToAlpha) {
                ToEntry alpha = FromAlpha ? static_cast<ToEntry>(rgb[3]) : static_cast<ToEntry>(1);
                return {static_cast<ToEntry>(rgb[0]), static_cast<ToEntry>(rgb[1]), static_cast<ToEntry>(rgb[2]),
                        alpha};
            } else {
                return {static_cast<ToEntry>(rgb[0]), static_cast<ToEntry>(rgb[1]), static_cast<ToEntry>(rgb[2])};
            }
        }
    };

    template<bool FromAlpha, bool ToAlpha>
    struct ColorTransformer<HSLColorSpace<FromAlpha>, HSLColorSpace<ToAlpha>>
    {
        using Data = HSLColorSpace<FromAlpha>::Data;
        using ToData = HSLColorSpace<ToAlpha>::Data;
        using Entry = HSLColorSpace<FromAlpha>::Entry;
        using ToEntry = HSLColorSpace<ToAlpha>::Entry;

        static ToData transform(const Data& hsl)
        {
            if constexpr (ToAlpha) {
                ToEntry alpha = FromAlpha ? static_cast<ToEntry>(hsl[3]) : static_cast<ToEntry>(1);
                return {static_cast<ToEntry>(hsl[0]), static_cast<ToEntry>(hsl[1]), static_cast<ToEntry>(hsl[2]),
                        alpha};
            } else {
                return {static_cast<ToEntry>(hsl[0]), static_cast<ToEntry>(hsl[1]), static_cast<ToEntry>(hsl[2])};
            }
        }
    };

    template<bool HSLAlpha, bool RGBAlpha>
    struct ColorTransformer<HSLColorSpace<HSLAlpha>, RGBColorSpace<RGBAlpha>>
    {
        using Data = HSLColorSpace<HSLAlpha>::Data;
        using ToData = RGBColorSpace<RGBAlpha>::Data;
        using Entry = HSLColorSpace<HSLAlpha>::Entry;
        using ToEntry = RGBColorSpace<RGBAlpha>::Entry;

        static ToData transform(const Data& hsl)
        {
            Entry h = hsl[0];
            Entry s = hsl[1];
            Entry l = hsl[2];

            if (s == static_cast<Entry>(0)) {
                if constexpr (RGBAlpha) {
                    ToEntry alpha = HSLAlpha ? static_cast<ToEntry>(hsl[3]) : static_cast<ToEntry>(1);
                    return {l, l, l, alpha};
                } else {
                    return {l, l, l};
                }
            }

            const Entry c = (static_cast<Entry>(1) - std::abs(static_cast<Entry>(2) * l - static_cast<Entry>(1))) * s;
            const Entry hPrime = std::fmod(h * static_cast<Entry>(6), static_cast<Entry>(6));
            const Entry x = c * (static_cast<Entry>(1) -
                                 std::abs(std::fmod(hPrime, static_cast<Entry>(2)) - static_cast<Entry>(1)));
            const Entry m = l - c / static_cast<Entry>(2);

            Entry r, g, b;
            if (hPrime < static_cast<Entry>(1)) {
                std::tie(r, g, b) = std::make_tuple(c, x, static_cast<Entry>(0));
            } else if (hPrime < static_cast<Entry>(2)) {
                std::tie(r, g, b) = std::make_tuple(x, c, static_cast<Entry>(0));
            } else if (hPrime < static_cast<Entry>(3)) {
                std::tie(r, g, b) = std::make_tuple(static_cast<Entry>(0), c, x);
            } else if (hPrime < static_cast<Entry>(4)) {
                std::tie(r, g, b) = std::make_tuple(static_cast<Entry>(0), x, c);
            } else if (hPrime < static_cast<Entry>(5)) {
                std::tie(r, g, b) = std::make_tuple(x, static_cast<Entry>(0), c);
            } else {
                std::tie(r, g, b) = std::make_tuple(c, static_cast<Entry>(0), x);
            }

            if constexpr (RGBAlpha) {
                ToEntry alpha = HSLAlpha ? static_cast<ToEntry>(hsl[3]) : static_cast<ToEntry>(1);
                return {static_cast<ToEntry>(r + m), static_cast<ToEntry>(g + m), static_cast<ToEntry>(b + m), alpha};
            } else {
                return {static_cast<ToEntry>(r + m), static_cast<ToEntry>(g + m), static_cast<ToEntry>(b + m)};
            }
        }
    };

    template<bool RGBAlpha, bool HSLAlpha>
    struct ColorTransformer<RGBColorSpace<RGBAlpha>, HSLColorSpace<HSLAlpha>>
    {
        using Data = RGBColorSpace<RGBAlpha>::Data;
        using ToData = HSLColorSpace<HSLAlpha>::Data;
        using Entry = RGBColorSpace<RGBAlpha>::Entry;
        using ToEntry = HSLColorSpace<HSLAlpha>::Entry;

        static ToData transform(const Data& rgb)
        {
            Entry r = rgb[0];
            Entry g = rgb[1];
            Entry b = rgb[2];

            const Entry cMax = std::max({r, g, b});
            const Entry cMin = std::min({r, g, b});
            const Entry delta = cMax - cMin;

            Entry h = static_cast<Entry>(0);
            Entry s = static_cast<Entry>(0);
            Entry l = (cMax + cMin) / static_cast<Entry>(2);

            // Check for grayscale
            if (delta > static_cast<Entry>(FLT_EPSILON)) {
                // Calculate Saturation
                s = delta / (static_cast<Entry>(1) - std::abs(static_cast<Entry>(2) * l - static_cast<Entry>(1)));

                // Calculate Hue
                if (cMax == r) {
                    h = std::fmod((g - b) / delta, static_cast<Entry>(6));
                } else if (cMax == g) {
                    h = (b - r) / delta + static_cast<Entry>(2);
                } else {
                    h = (r - g) / delta + static_cast<Entry>(4);
                }

                // Normalize H to [0, 1]
                h /= static_cast<Entry>(6);
                if (h < static_cast<Entry>(0)) {
                    h += static_cast<Entry>(1);
                }
            }

            if constexpr (HSLAlpha) {
                ToEntry alpha = RGBAlpha ? static_cast<ToEntry>(rgb[3]) : static_cast<ToEntry>(1);
                return {static_cast<ToEntry>(h), static_cast<ToEntry>(s), static_cast<ToEntry>(l), alpha};
            } else {
                return {static_cast<ToEntry>(h), static_cast<ToEntry>(s), static_cast<ToEntry>(l)};
            }
        }
    };

    template<bool HSVAlpha, bool RGBAlpha>
    struct ColorTransformer<HSVColorSpace<HSVAlpha>, RGBColorSpace<RGBAlpha>>
    {
        using Data = HSVColorSpace<HSVAlpha>::Data;
        using ToData = RGBColorSpace<RGBAlpha>::Data;
        using Entry = HSVColorSpace<HSVAlpha>::Entry;
        using ToEntry = RGBColorSpace<RGBAlpha>::Entry;

        static ToData transform(const Data& hsv)
        {
            Entry h = hsv[0];
            Entry s = hsv[1];
            Entry v = hsv[2];
            // Default to grayscale (v)
            Entry r = v, g = v, b = v;

            if (s > static_cast<Entry>(FLT_EPSILON)) {
                Entry h_prime = std::fmod(h * static_cast<Entry>(6), static_cast<Entry>(6));
                int i = static_cast<int>(h_prime);
                Entry f = h_prime - static_cast<Entry>(i);
                Entry p = v * (static_cast<Entry>(1) - s);
                Entry q = v * (static_cast<Entry>(1) - f * s);
                Entry t = v * (static_cast<Entry>(1) - (static_cast<Entry>(1) - f) * s);

                switch (i) {
                    case 0:
                        std::tie(r, g, b) = std::make_tuple(v, t, p);
                        break;
                    case 1:
                        std::tie(r, g, b) = std::make_tuple(q, v, p);
                        break;
                    case 2:
                        std::tie(r, g, b) = std::make_tuple(p, v, t);
                        break;
                    case 3:
                        std::tie(r, g, b) = std::make_tuple(p, q, v);
                        break;
                    case 4:
                        std::tie(r, g, b) = std::make_tuple(t, p, v);
                        break;
                    case 5:
                    default:
                        std::tie(r, g, b) = std::make_tuple(v, p, q);
                        break;
                }
            }

            if constexpr (RGBAlpha) {
                ToEntry alpha = HSVAlpha ? static_cast<ToEntry>(hsv[3]) : static_cast<ToEntry>(1);
                return {static_cast<ToEntry>(r), static_cast<ToEntry>(g), static_cast<ToEntry>(b), alpha};
            } else {
                return {static_cast<ToEntry>(r), static_cast<ToEntry>(g), static_cast<ToEntry>(b)};
            }
        }
    };

    template<bool RGBAlpha, bool HSVAlpha>
    struct ColorTransformer<RGBColorSpace<RGBAlpha>, HSVColorSpace<HSVAlpha>>
    {
        using Data = RGBColorSpace<RGBAlpha>::Data;
        using ToData = HSVColorSpace<HSVAlpha>::Data;
        using Entry = RGBColorSpace<RGBAlpha>::Entry;
        using ToEntry = HSVColorSpace<HSVAlpha>::Entry;

        static ToData transform(const Data& rgb)
        {
            Entry r = rgb[0];
            Entry g = rgb[1];
            Entry b = rgb[2];

            const Entry cMax = std::max({r, g, b});
            const Entry cMin = std::min({r, g, b});
            const Entry delta = cMax - cMin;

            Entry h = static_cast<Entry>(0);
            Entry s = static_cast<Entry>(0);
            Entry v = cMax;

            if (delta > static_cast<Entry>(FLT_EPSILON)) {
                // Calculate Hue (same as HSL)
                if (cMax == r) {
                    h = std::fmod((g - b) / delta, static_cast<Entry>(6));
                } else if (cMax == g) {
                    h = (b - r) / delta + static_cast<Entry>(2);
                } else {
                    h = (r - g) / delta + static_cast<Entry>(4);
                }
                h /= static_cast<Entry>(6);
                if (h < static_cast<Entry>(0)) {
                    h += static_cast<Entry>(1);
                }

                // Calculate Saturation (different from HSL)
                if (cMax > static_cast<Entry>(FLT_EPSILON)) {
                    s = delta / cMax;
                }
            }

            if constexpr (HSVAlpha) {
                ToEntry alpha = RGBAlpha ? static_cast<ToEntry>(rgb[3]) : static_cast<ToEntry>(1);
                return {static_cast<ToEntry>(h), static_cast<ToEntry>(s), static_cast<ToEntry>(v), alpha};
            } else {
                return {static_cast<ToEntry>(h), static_cast<ToEntry>(s), static_cast<ToEntry>(v)};
            }
        }
    };

    template<bool HSLAlpha, bool HSVAlpha>
    struct ColorTransformer<HSLColorSpace<HSLAlpha>, HSVColorSpace<HSVAlpha>>
    {
        using Data = HSLColorSpace<HSLAlpha>::Data;
        using ToData = HSVColorSpace<HSVAlpha>::Data;
        using Entry = HSLColorSpace<HSLAlpha>::Entry;
        using ToEntry = HSVColorSpace<HSVAlpha>::Entry;

        static ToData transform(const Data& hsl)
        {
            Entry h = hsl[0];
            Entry s = hsl[1];
            Entry l = hsl[2];

            Entry v = l + s * std::min(l, static_cast<Entry>(1) - l);
            Entry sHSV;

            if (v < static_cast<Entry>(FLT_EPSILON)) {
                sHSV = static_cast<Entry>(0);
            } else {
                sHSV = static_cast<Entry>(2) * (static_cast<Entry>(1) - l / v);
            }

            if constexpr (HSVAlpha) {
                ToEntry alpha = HSLAlpha ? static_cast<ToEntry>(hsl[3]) : static_cast<ToEntry>(1);
                return {static_cast<ToEntry>(h), static_cast<ToEntry>(sHSV), static_cast<ToEntry>(v), alpha};
            } else {
                return {static_cast<ToEntry>(h), static_cast<ToEntry>(sHSV), static_cast<ToEntry>(v)};
            }
        }
    };

    template<bool HSVAlpha, bool HSLAlpha>
    struct ColorTransformer<HSVColorSpace<HSVAlpha>, HSLColorSpace<HSLAlpha>>
    {
        using Data = HSVColorSpace<HSVAlpha>::Data;
        using ToData = HSLColorSpace<HSLAlpha>::Data;
        using Entry = HSVColorSpace<HSVAlpha>::Entry;
        using ToEntry = HSLColorSpace<HSLAlpha>::Entry;

        static ToData transform(const Data& hsv)
        {
            Entry h = hsv[0];
            Entry s = hsv[1];
            Entry v = hsv[2];

            Entry l = v * (static_cast<Entry>(1) - s / static_cast<Entry>(2));
            Entry sHSL;

            if (l < static_cast<Entry>(FLT_EPSILON) || std::abs(l - 1.0f) < static_cast<Entry>(FLT_EPSILON)) {
                sHSL = static_cast<Entry>(0);
            } else {
                sHSL = (v - l) / std::min(l, static_cast<Entry>(1) - l);
            }

            if constexpr (HSLAlpha) {
                ToEntry alpha = HSVAlpha ? static_cast<ToEntry>(hsv[3]) : static_cast<ToEntry>(1);
                return {static_cast<ToEntry>(h), static_cast<ToEntry>(sHSL), static_cast<ToEntry>(l), alpha};
            } else {
                return {static_cast<ToEntry>(h), static_cast<ToEntry>(sHSL), static_cast<ToEntry>(l)};
            }
        }
    };
} // namespace neon

#endif // NEON_COLORSPACETRANSFORMATIONS_H
