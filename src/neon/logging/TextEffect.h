//
// Created by gaeqs on 8/09/24.
//

#ifndef MESSAGEMODIFIER_H
#define MESSAGEMODIFIER_H

#include <cstdint>
#include <rush/rush.h>

namespace neon
{
    enum class TextColor4Bits : uint8_t
    {
        BLACK = 0,
        RED = 1,
        GREEN = 2,
        YELLOW = 3,
        BLUE = 4,
        MAGENTA = 5,
        CYAN = 6,
        WHITE = 7,
        BRIGHT_BLACK = 8,
        BRIGHT_RED = 9,
        BRIGHT_GREEN = 10,
        BRIGHT_YELLOW = 11,
        BRIGHT_BLUE = 12,
        BRIGHT_MAGENTA = 13,
        BRIGHT_CYAN = 14,
        BRIGHT_WHITE = 15,
    };

    /**
     * Represents a text effect you can use on messages.
     */
    struct TextEffect
    {
        uint8_t code;
        uint8_t metadata[4];

        [[nodiscard]] std::optional<rush::Vec3f> toRGB() const;

        static TextEffect reset();

        static TextEffect bold();

        static TextEffect faint();

        static TextEffect italic();

        static TextEffect underline();

        static TextEffect slowBlink();

        static TextEffect fastBlink();

        static TextEffect conceal();

        static TextEffect crossedOut();

        static TextEffect font(uint8_t font);

        static TextEffect fraktrur();

        static TextEffect foreground4bits(uint8_t color);

        static TextEffect foreground4bits(TextColor4Bits color);

        static TextEffect background4bits(uint8_t color);

        static TextEffect background4bits(TextColor4Bits color);

        static TextEffect foreground8bits(uint8_t color);

        static TextEffect background8bits(uint8_t color);

        static TextEffect foregroundRGB(uint8_t red, uint8_t green, uint8_t blue);

        static TextEffect backgroundRGB(uint8_t red, uint8_t green, uint8_t blue);

        static TextEffect defaultForeground();

        static TextEffect defaultBackground();

        static TextEffect framed();

        static TextEffect encircled();

        static TextEffect overlined();

      private:
        [[nodiscard]] std::optional<rush::Vec3f> toRGBFromMetadata() const;
    };
} // namespace neon

#endif //MESSAGEMODIFIER_H
