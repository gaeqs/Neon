//
// Created by gaeqs on 16/11/25.
//

#ifndef NEON_COLOR_H
#define NEON_COLOR_H

#include <imgui.h>
#include <rush/vector/vec.h>

#include <neon/color/RGBColorSpace.h>
#include <neon/color/ColorSpaceTransformations.h>

namespace neon
{

    // region Concepts
    /**
     * @concept ColorHasUInt32Constructor
     * @brief Concept for ColorSpaces that can be constructed from a 32-bit integer.
     */
    template<typename ColorSpace>
    concept ColorHasUInt32Constructor = requires(uint32_t i) {
        { ColorSpace::fromUInt32(i) } -> std::convertible_to<typename ColorSpace::Data>;
    };

    /**
     * @concept ColorHasUIntsConstructor
     * @brief Concept for ColorSpaces that can be constructed from a variadic list of integers.
     */
    template<typename ColorSpace, typename... T>
    concept ColorHasUIntsConstructor = requires(T... i) {
        { ColorSpace::fromUInts(i...) } -> std::convertible_to<typename ColorSpace::Data>;
    };

    /**
     * @concept ColorHasFloatsConstructor
     * @brief Concept for ColorSpaces that can be constructed from a variadic list of floats.
     */
    template<typename ColorSpace, typename... T>
    concept ColorHasFloatsConstructor = requires(T... i) {
        { ColorSpace::fromFloats(i...) } -> std::convertible_to<typename ColorSpace::Data>;
    };

    /**
     * @concept ColorSpaceHasLighter
     * @brief Concept for ColorSpaces that support a 'lighter' operation.
     */
    template<typename ColorSpace>
    concept ColorSpaceHasLighter = requires(typename ColorSpace::Data d, typename ColorSpace::Entry amount) {
        { ColorSpace::lighter(d, amount) } -> std::convertible_to<typename ColorSpace::Data>;
    };

    /**
     * @concept ColorSpaceHasDarker
     * @brief Concept for ColorSpaces that support a 'darker' operation.
     */
    template<typename ColorSpace>
    concept ColorSpaceHasDarker = requires(typename ColorSpace::Data d, typename ColorSpace::Entry amount) {
        { ColorSpace::darker(d, amount) } -> std::convertible_to<typename ColorSpace::Data>;
    };

    /**
     * @concept ColorSpaceHasRotateHue
     * @brief Concept for ColorSpaces that support hue rotation.
     */
    template<typename ColorSpace>
    concept ColorSpaceHasRotateHue = requires(typename ColorSpace::Data d, typename ColorSpace::Entry angle) {
        { ColorSpace::rotateHue(d, angle) } -> std::convertible_to<typename ColorSpace::Data>;
    };

    /**
     * @concept ColorSpaceHasComplementary
     * @brief Concept for ColorSpaces that can find a complementary color.
     */
    template<typename ColorSpace>
    concept ColorSpaceHasComplementary = requires(typename ColorSpace::Data d) {
        { ColorSpace::complementary(d) } -> std::convertible_to<typename ColorSpace::Data>;
    };

    /**
     * @concept ColorSpaceHasTriadic
     * @brief Concept for ColorSpaces that can find a triadic color.
     */
    template<typename ColorSpace>
    concept ColorSpaceHasTriadic = requires(typename ColorSpace::Data d) {
        { ColorSpace::triadic(d) } -> std::convertible_to<typename ColorSpace::Data>;
    };

    /**
     * @concept ColorSpaceHasToUInt
     * @brief Concept for ColorSpaces that can be exported to a 32-bit integer.
     */
    template<typename ColorSpace>
    concept ColorSpaceHasToUInt = requires(typename ColorSpace::Data d) {
        { ColorSpace::toUInt(d) } -> std::convertible_to<uint32_t>;
    };

    /**
     * @concept ColorSpaceHasTransformation
     * @brief Concept checking if a transformation exists between two ColorSpaces.
     */
    template<typename CS1, typename CS2>
    concept ColorSpaceHasTransformation = requires(typename CS1::Data d1) {
        { ColorTransformer<CS1, CS2>::transform(d1) } -> std::convertible_to<typename CS2::Data>;
    };

    /**
     * @concept ColorHasPrint
     * @brief Concept for ColorSpaces that implement a static 'print' method.
     */
    template<typename ColorSpace>
    concept ColorHasPrint = requires(std::ostream& os, typename ColorSpace::Data d) { ColorSpace::print(os, d); };

    // endregion

    /**
     * @class Color
     * @brief A type-safe, generic color container.
     *
     * This class acts as a high-level wrapper for color data. It is templated
     * on a `ColorSpace` (like RGBColorSpace, HSLColorSpace) which defines
     * the underlying data and available operations.
     *
     * The class uses concepts to enable/disable methods based on the
     * capabilities of its ColorSpace (for example, '::lighter()' is only available
     * if the ColorSpace implements it).
     *
     * @tparam ColorSpace The color space policy struct.
     * Defaults to RGBColorSpace<false> (non-alpha).
     */
    template<typename ColorSpace = RGBColorSpace<>>
    class Color
    {
        ColorSpace::Data data;

      public:
        /**
         * @brief Default constructor. Initializes an empty/default color.
         */
        Color() = default;

        /**
         * @brief Constructs a color from its raw ColorSpace data.
         * @param data The raw data vector.
         */
        Color(ColorSpace::Data data) :
            data(std::move(data))
        {
        }

        /**
         * @brief Constructs a color from a list of integer components.
         * @details Enabled by the ColorHasUIntsConstructor concept.
         * For RGB, this is typically (r, g, b) as 0-255.
         * @tparam T Variadic integral types.
         * @param data The color components.
         */
        template<typename... T>
            requires ColorHasUIntsConstructor<ColorSpace, T...>
        Color(T... data) :
            data(ColorSpace::fromUInts(data...))
        {
        }

        /**
         * @brief Constructs a color from a variadic list of floating-point components.
         * @details Enabled by the ColorHasFloatsConstructor concept.
         * For RGB/HSL/HSV, this is typically (c1, c2, c3) as 0.0-1.0.
         * @tparam T Variadic floating-point types.
         * @param data The color components.
         */
        template<typename... T>
            requires ColorHasFloatsConstructor<ColorSpace, T...>
        Color(T... data) :
            data(ColorSpace::fromFloats(data...))
        {
        }

        /**
         * @brief Constructs a color from a single 32-bit integer.
         * @details Enabled by the ColorHasUInt32Constructor concept.
         * For RGB, this typically expects 0xAARRGGBB format.
         * @param data The 32-bit integer color value.
         */
        Color(uint32_t data)
            requires ColorHasUInt32Constructor<ColorSpace>
            :
            data(ColorSpace::fromUInt32(data))
        {
        }

        /**
         * @brief Converting constructor from another ColorSpace.
         *
         * Allows implicit conversion between color types (for example, HSL to RGB).
         * This constructor is disabled if 'OtherColorSpace' is the same
         * as 'ColorSpace' to avoid conflicting with the copy constructor.
         *
         * @tparam OtherColorSpace The ColorSpace of the 'other' color.
         * @param other The Color object to convert from.
         */
        template<typename OtherColorSpace>
            requires ColorSpaceHasTransformation<OtherColorSpace, ColorSpace> &&
                     (!std::is_same_v<ColorSpace, OtherColorSpace>)
        Color(const Color<OtherColorSpace>& other) :
            data(ColorTransformer<OtherColorSpace, ColorSpace>::transform(other.getData()))
        {
        }

        /**
         * @brief Returns a mutable reference to the underlying data.
         * @return A reference to the ColorSpace::Data.
         */
        ColorSpace::Data& getData()
        {
            return data;
        }

        /**
         * @brief Returns a const reference to the underlying data.
         * @return A reference to the ColorSpace::Data.
         */
        const ColorSpace::Data& getData() const
        {
            return data;
        }

        /**
         * @brief Provides mutable access to individual color components.
         * @param i The component index (e.g., 0 for R, 1 for G, ...).
         * @return A mutable reference to the component.
         */
        ColorSpace::Entry& operator[](size_t i)
        {
            return data[i];
        }

        /**
         * @brief Provides const access to individual color components.
         * @param i The component index (e.g., 0 for R, 1 for G, ...).
         * @return A const reference to the component.
         */
        const ColorSpace::Entry& operator[](size_t i) const
        {
            return data[i];
        }

        /**
         * @brief Linearly interpolates (mixes) this color with another.
         * @param other The target color to mix towards.
         * @param t The interpolation factor (0.0 = this color, 1.0 = other color).
         * @return The new interpolated Color.
         */
        Color mix(const Color& other, const ColorSpace::Entry& t) const
        {
            return Color(rush::mix(data, other.data, t));
        }

        /**
         * @brief Returns a lighter version of this color.
         * @details Only enabled for ColorSpaces that support it (like HSL).
         * @param amount The amount to lighten (typically 0.0 to 1.0).
         * @return A new, lighter Color.
         */
        Color lighter(const ColorSpace::Entry& amount) const
            requires ColorSpaceHasLighter<ColorSpace>
        {
            return Color(ColorSpace::lighter(data, amount));
        }

        /**
         * @brief Returns a darker version of this color.
         * @details Only enabled for ColorSpaces that support it (like HSL).
         * @param amount The amount to darken (typically 0.0 to 1.0).
         * @return A new, darker Color.
         */
        Color darker(const ColorSpace::Entry& amount) const
            requires ColorSpaceHasDarker<ColorSpace>
        {
            return Color(ColorSpace::darker(data, amount));
        }

        /**
         * @brief Rotates the hue of this color.
         * @details Only enabled for ColorSpaces that support it (like HSL).
         * @param angle The angle to rotate, normalized (0.0 to 1.0).
         * @return A new Color with the rotated hue.
         */
        Color rotateHue(const ColorSpace::Entry& angle) const
            requires ColorSpaceHasRotateHue<ColorSpace>
        {
            return Color(ColorSpace::rotateHue(data, angle));
        }

        /**
         * @brief Finds the complementary color (180-degree hue rotation).
         * @details Only enabled for ColorSpaces that support it (like HSL).
         * @return The complementary Color.
         */
        Color complementary() const
            requires ColorSpaceHasComplementary<ColorSpace>
        {
            return Color(ColorSpace::complementary(data));
        }

        /**
         * @brief Finds a triadic color (120-degree hue rotation).
         * @details Only enabled for ColorSpaces that support it (like HSL).
         * @return A triadic Color.
         */
        Color triadic() const
            requires ColorSpaceHasTriadic<ColorSpace>
        {
            return Color(ColorSpace::triadic(data));
        }

        /**
         * @brief Exports the color to a 32-bit integer (e.g., 0xAARRGGBB).
         * @details Only enabled for ColorSpaces that support it (like RGB).
         * If the color doesn't implement an alpha channel,
         * the alpha value will always be 0xFF.
         * @return The 32-bit ARGB integer representation.
         */
        uint32_t toUInt() const
            requires ColorSpaceHasToUInt<ColorSpace>
        {
            return ColorSpace::toUInt(data);
        }

        /**
         * @brief Exports the color to a 32-bit integer in ImGui's format (ABGR).
         * @details This method is robust and works for any ColorSpace.
         * If the color is not already in an RGB-based space, it will be
         * automatically transformed to RGBA first.
         * It performs the necessary (ARGB -> ABGR) byte-swap.
         * @return The 32-bit ABGR uint32_t.
         */
        uint32_t toImGuiUInt() const
            requires(std::is_same_v<ColorSpace, RGBAColorSpace> || std::is_same_v<ColorSpace, RGBColorSpace<>> ||
                     ColorSpaceHasTransformation<ColorSpace, RGBAColorSpace>)
        {
            if constexpr (std::is_same_v<ColorSpace, RGBAColorSpace> || std::is_same_v<ColorSpace, RGBColorSpace<>>) {
                uint32_t argb = toUInt();
                return argb & 0xFF00FF00 | (argb & 0x00FF0000) >> 16 | (argb & 0x000000FF) << 16;
            } else {
                return transformTo<RGBAColorSpace>().toImGuiUInt();
            }
        }

        /**
         * @brief Exports the color to an ImVec4 (float vector) for ImGui.
         * @details This method is robust and works for any ColorSpace.
         * If the color is not already in an RGB-based space, it will be
         * automatically transformed to RGBA first.
         * If the color is RGB (no alpha), an alpha of 1.0 (opaque)
         * will be supplied.
         * @return An ImVec4 struct {r, g, b, a} with components in [0.0, 1.0].
         */
        ImVec4 toImGuiVector() const
            requires(std::is_same_v<ColorSpace, RGBAColorSpace> || std::is_same_v<ColorSpace, RGBColorSpace<>> ||
                     ColorSpaceHasTransformation<ColorSpace, RGBAColorSpace>)
        {
            if constexpr (std::is_same_v<ColorSpace, RGBAColorSpace>) {
                return {data[0], data[1], data[2], data[3]};
            } else if constexpr (std::is_same_v<ColorSpace, RGBColorSpace<>>) {
                return {data[0], data[1], data[2], 1.0f};
            } else {
                return transformTo<RGBAColorSpace>().toImGuiVector();
            }
        }

        /**
         * @brief Transforms this color into a different color space.
         * @tparam OtherColorSpace The target ColorSpace to transform to.
         * @return A new Color object in the target color space.
         */
        template<typename OtherColorSpace>
            requires ColorSpaceHasTransformation<ColorSpace, OtherColorSpace>
        Color<OtherColorSpace> transformTo() const
        {
            return Color<OtherColorSpace>(ColorTransformer<ColorSpace, OtherColorSpace>::transform(data));
        }

        /**
         * @brief Checks if this color is "nearly equal" to another of the same type.
         * @details This performs a floating-point comparison using an epsilon.
         * @param other The other color to compare against.
         * @param epsilon The tolerance for the comparison.
         * @return True if all components are nearly equal, false otherwise.
         */
        bool isNearlyEqual(const Color& other,
                           ColorSpace::Entry epsilon = static_cast<ColorSpace::Entry>(FLT_EPSILON)) const
        {
            for (size_t i = 0; i < ColorSpace::EntriesAmount; ++i) {
                if (std::abs(data[i] - other.data[i]) > epsilon) {
                    return false;
                }
            }
            return true;
        }
    };

    /**
     * @brief Heterogeneous, "nearly equal" equality operator.
     *
     * Compares two colors for visual equality.
     * - If they are the same type (RGB vs. RGB), compares components with an epsilon.
     * - If they are different types (RGB vs. HSL), converts both to a
     * canonical RGBA space and compares them there.
     *
     * @tparam CS1 The ColorSpace of the left-hand side.
     * @tparam CS2 The ColorSpace of the right-hand side.
     * @param l The left-hand side Color.
     * @param r The right-hand side Color.
     * @return True if the colors are visually equal, false otherwise.
     */
    template<typename CS1, typename CS2>
        requires(ColorSpaceHasTransformation<CS1, RGBAColorSpace> &&
                 ColorSpaceHasTransformation<CS2, RGBAColorSpace>) ||
                std::is_same_v<CS1, CS2>
    bool operator==(const Color<CS1>& l, const Color<CS2>& r)
    {
        if constexpr (std::is_same_v<CS1, CS2>) {
            return l.isNearlyEqual(r);
        } else {
            // We use RGBAColorSpace as the canonical space.
            auto rgbaL = l.template transformTo<RGBAColorSpace>();
            auto rgbaR = r.template transformTo<RGBAColorSpace>();
            return rgbaL.isNearlyEqual(rgbaR);
        }
    }

    namespace colors
    {
        inline const Color Red(255, 0, 0);
        inline const Color Green(0, 255, 0);
        inline const Color Blue(0, 0, 255);

        inline const Color Yellow(255, 255, 0);
        inline const Color Cyan(0, 255, 255);

        inline const Color White(255, 255, 255);
        inline const Color Black(0, 0, 0);
        inline const Color Gray(128, 128, 128);
        inline const Color LightGray(211, 211, 211);
        inline const Color DarkGray(169, 169, 169);

        inline const Color Pink(255, 192, 203);
        inline const Color LightPink(255, 182, 193);
        inline const Color HotPink(255, 105, 180);
        inline const Color DeepPink(255, 20, 147);
        inline const Color Magenta(255, 0, 255);

        inline const Color LimeGreen(50, 205, 50);
        inline const Color ForestGreen(34, 139, 34);
        inline const Color Olive(128, 128, 0);
        inline const Color DarkGreen(0, 100, 0);
        inline const Color SeaGreen(46, 139, 87);

        inline const Color Aqua(0, 255, 255);
        inline const Color LightCyan(224, 255, 255);
        inline const Color Turquoise(64, 224, 208);
        inline const Color Teal(0, 128, 128);
        inline const Color DarkCyan(0, 139, 139);

        inline const Color LightSkyBlue(135, 206, 250);
        inline const Color SkyBlue(135, 206, 235);
        inline const Color SteelBlue(70, 130, 180);
        inline const Color RoyalBlue(65, 105, 225);
        inline const Color MidnightBlue(25, 25, 112);
        inline const Color Navy(0, 0, 128);

        inline const Color Purple(128, 0, 128);
        inline const Color Violet(238, 130, 238);
        inline const Color DarkViolet(148, 0, 211);
        inline const Color BlueViolet(138, 43, 226);
        inline const Color Indigo(75, 0, 130);
        inline const Color SlateBlue(106, 90, 205);

        inline const Color Brown(165, 42, 42);
        inline const Color Sienna(160, 82, 45);
        inline const Color SaddleBrown(139, 69, 19);
        inline const Color Chocolate(210, 105, 30);
        inline const Color Maroon(128, 0, 0);

        inline const Color<RGBColorSpace<true>> Transparent(0, 0, 0, 0);

    } // namespace colors

} // namespace neon

/**
 * @brief Stream insertion operator for printing a Color.
 * @details Relies on the ColorSpace's `print` static method.
 * @param os The output stream.
 * @param v The Color to print.
 * @return The output stream.
 */
template<typename ColorSpace>
    requires neon::ColorHasPrint<ColorSpace>
std::ostream& operator<<(std::ostream& os, const neon::Color<ColorSpace>& v)

{
    ColorSpace::print(os, v.getData());
    return os;
}

#endif // NEON_COLOR_H
