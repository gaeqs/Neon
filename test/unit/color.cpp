//
// Created by gaeqs on 24/08/24.
//

#include <iostream>
#include <catch2/catch_all.hpp>
#include <neon/color/Color.h>
#include <neon/color/HSLColorSpace.h>

using namespace neon;
using RGB = Color<RGBColorSpace<false>>;
using RGBA = Color<RGBColorSpace<true>>;
using HSL = Color<HSLColorSpace<false>>;
using HSLA = Color<HSLColorSpace<true>>;
using HSV = Color<HSVColorSpace<false>>;
using HSVA = Color<HSVColorSpace<true>>;

TEST_CASE("Color Construction", "[color]")
{
    // Use Catch2's Approx for float comparisons
    using Catch::Approx;

    SECTION("Default constructor")
    {
        // The default color is RGB<false>
        Color<> c;
        // Default constructed rush::Vec should be zero-initialized
        CHECK(c[0] == Approx(0.0f));
        CHECK(c[1] == Approx(0.0f));
        CHECK(c[2] == Approx(0.0f));
    }

    SECTION("RGB from uints (0-255)")
    {
        RGB r(255, 0, 0);
        CHECK(r[0] == Approx(1.0f));
        CHECK(r[1] == Approx(0.0f));
        CHECK(r[2] == Approx(0.0f));

        RGBA a(0, 128, 255, 128);
        CHECK(a[0] == Approx(0.0f));
        CHECK(a[1] == Approx(128 / 255.0f));
        CHECK(a[2] == Approx(1.0f));
        CHECK(a[3] == Approx(128 / 255.0f));
    }

    SECTION("Color from floats (0.0-1.0)")
    {
        HSL h(0.5f, 1.0f, 0.5f);
        CHECK(h[0] == Approx(0.5f));
        CHECK(h[1] == Approx(1.0f));
        CHECK(h[2] == Approx(0.5f));
    }

    SECTION("Color from uint32_t (ARGB)")
    {
        // Your fromUInt32 assumes ARGB (0xAARRGGBB)
        RGBA c(0x80FF0000);                  // 50% transparent Red
        CHECK(c[0] == Approx(1.0f));         // R
        CHECK(c[1] == Approx(0.0f));         // G
        CHECK(c[2] == Approx(0.0f));         // B
        CHECK(c[3] == Approx(128 / 255.0f)); // A
    }
}

TEST_CASE("Color Transformations & Comparison", "[color]")
{
    using Catch::Approx;

    // Define canonical colors in all spaces
    // These are the "ground truth" values we will test against.

    // --- PURE RED ---
    auto redRGB = RGB(255, 0, 0);
    auto redHSL = HSL(0.0f, 1.0f, 0.5f);
    auto redHSV = HSV(0.0f, 1.0f, 1.0f);

    // --- PURE GREEN ---
    auto greenRGB = RGB(0, 255, 0);
    auto greenHSL = HSL(1.0f / 3.0f, 1.0f, 0.5f);
    auto greenHSV = HSV(1.0f / 3.0f, 1.0f, 1.0f);

    // --- PURE BLUE ---
    auto blueRGB = RGB(0, 0, 255);
    auto blueHSL = HSL(2.0f / 3.0f, 1.0f, 0.5f);
    auto blueHSV = HSV(2.0f / 3.0f, 1.0f, 1.0f);

    // --- WHITE ---
    auto whiteRGB = RGB(255, 255, 255);
    auto whiteHSL = HSL(0.0f, 0.0f, 1.0f);
    auto whiteHSV = HSV(0.0f, 0.0f, 1.0f);

    // --- BLACK ---
    auto blackRGB = RGB(0, 0, 0);
    auto blackHSL = HSL(0.0f, 0.0f, 0.0f);
    auto blackHSV = HSV(0.0f, 0.0f, 0.0f);

    // --- GRAY ---
    auto grayRGB = RGB(128, 128, 128);
    auto grayHSL = HSL(0.0f, 0.0f, 128 / 255.0f);
    auto grayHSV = HSV(0.0f, 0.0f, 128 / 255.0f);

    SECTION("Heterogeneous Comparison (operator==)")
    {
        REQUIRE(redRGB == redHSL);
        REQUIRE(redHSL == redHSV);
        REQUIRE(redRGB == redHSV);

        REQUIRE(greenRGB == greenHSL);
        REQUIRE(greenRGB == greenHSV);

        REQUIRE(blueRGB == blueHSL);
        REQUIRE(blueRGB == blueHSV);

        REQUIRE(whiteRGB == whiteHSL);
        REQUIRE(whiteRGB == whiteHSV);

        REQUIRE(blackRGB == blackHSL);
        REQUIRE(blackRGB == blackHSV);

        REQUIRE(grayRGB == grayHSL);
        REQUIRE(grayRGB == grayHSV);

        REQUIRE(redRGB != blueRGB);
        REQUIRE(redHSL != whiteHSV);
    }

    SECTION("Round-trip transformations")
    {
        // A color, when transformed and transformed back, should be equal to itself.
        auto roundTripHSL = redRGB.transformTo<HSLColorSpace<>>().transformTo<RGBColorSpace<>>();
        REQUIRE(redRGB == roundTripHSL);

        auto roundTripHSV = redRGB.transformTo<HSVColorSpace<>>().transformTo<RGBColorSpace<>>();
        REQUIRE(redRGB == roundTripHSV);

        auto roundTripRGB = redHSL.transformTo<RGBColorSpace<>>().transformTo<HSLColorSpace<>>();
        REQUIRE(redHSL == roundTripRGB);
    }

    SECTION("Alpha channel transformations")
    {
        RGBA rgba(255, 0, 0, 128);
        auto hsla = rgba.transformTo<HSLAColorSpace>();

        CHECK(hsla[0] == Approx(0.0f));
        CHECK(hsla[1] == Approx(1.0f));
        CHECK(hsla[2] == Approx(0.5f));
        CHECK(hsla[3] == Approx(128 / 255.0f)); // Alpha is preserved

        auto newRGBa = hsla.transformTo<RGBAColorSpace>();
        REQUIRE(rgba == newRGBa);
    }
}

TEST_CASE("Color Operations", "[color]")
{
    using Catch::Approx;

    SECTION("HSL Operations")
    {
        HSL c(0.5f, 1.0f, 0.5f); // Pure Cyan

        auto lighter = c.lighter(0.2f);
        CHECK(lighter[2] == Approx(0.7f));

        auto darker = c.darker(0.3f);
        CHECK(darker[2] == Approx(0.2f));

        // Test clamping
        auto maxLight = c.lighter(1.0f);
        CHECK(maxLight[2] == Approx(1.0f));
        auto maxDark = c.darker(1.0f);
        CHECK(maxDark[2] == Approx(0.0f));

        // Test hue rotation (0.5 + 0.25 = 0.75)
        auto rotated = c.rotateHue(0.25f);
        CHECK(rotated[0] == Approx(0.75f));

        // Test hue rotation (wrap-around)
        auto wrapped = c.rotateHue(0.6f); // 0.5 + 0.6 = 1.1 -> 0.1
        CHECK(wrapped[0] == Approx(0.1f));

        // Test hue rotation reverse (wrap-around)
        auto wrappedReversed = c.rotateHue(-0.6f); // 0.5 - 0.6 = -0.1 -> 0.0
        CHECK(wrappedReversed[0] == Approx(0.9f));

        // Test complementary (0.5 -> 0.0)
        auto comp = c.complementary();
        CHECK(comp[0] == Approx(0.0f));
        REQUIRE(comp == HSL(0.0f, 1.0f, 0.5f)); // Should be pure Red
    }

    SECTION("Binary Operations (mix)")
    {
        RGB red(255, 0, 0);
        RGB blue(0, 0, 255);

        // t = 0.0 should return 'red'
        REQUIRE(red.mix(blue, 0.0f) == red);
        // t = 1.0 should return 'blue'
        REQUIRE(red.mix(blue, 1.0f) == blue);

        // t = 0.5 should be purple
        auto purple = red.mix(blue, 0.5f);
        CHECK(purple[0] == Approx(0.5f));
        CHECK(purple[1] == Approx(0.0f));
        CHECK(purple[2] == Approx(0.5f));
        REQUIRE(purple == RGB(0.5f, 0.0f, 0.5f));
    }
}

TEST_CASE("Color Export", "[color]")
{
    SECTION("toUInt (ARGB)")
    {
        // Opaque Red
        RGB red(255, 0, 0);
        REQUIRE(red.toUInt() == 0xFFFF0000); // Alpha defaults to FF

        // 50% Transparent Green
        RGBA green(0, 255, 0, 128);
        REQUIRE(green.toUInt() == 0x8000FF00); // 0x80 = 128
    }

    SECTION("toImGuiColor (ABGR)")
    {
        // Opaque Red (FF, 00, 00) -> ARGB: 0xFFFF0000
        // ABGR: A=FF, B=00, G=00, R=FF -> 0xFF0000FF
        RGB red(255, 0, 0);
        REQUIRE(red.toImGuiColor() == 0xFF0000FF);

        // 50% Transparent Green (00, FF, 00, 80) -> ARGB: 0x8000FF00
        // ABGR: A=80, B=00, G=FF, R=00 -> 0x8000FF00
        RGBA green(0, 255, 0, 128);
        REQUIRE(green.toImGuiColor() == 0x8000FF00);

        // 50% Transparent Blue (00, 00, FF, 80) -> ARGB: 0x800000FF
        // ABGR: A=80, B=FF, G=00, R=00 -> 0x80FF0000
        RGBA blue(0, 0, 255, 128);
        REQUIRE(blue.toImGuiColor() == 0x80FF0000);
    }
}