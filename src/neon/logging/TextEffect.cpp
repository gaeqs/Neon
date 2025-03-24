//
// Created by gaeqs on 8/09/24.
//

#include "TextEffect.h"

namespace neon
{
    std::optional<rush::Vec3f> TextEffect::toRGB() const
    {
        switch (code) {
            case 30:
            case 40:
                return {
                    {0, 0, 0}
                };
            case 31:
            case 41:
                return {
                    {170, 0, 0}
                };
            case 32:
            case 42:
                return {
                    {0, 170, 0}
                };
            case 33:
            case 43:
                return {
                    {170, 85, 0}
                };
            case 34:
            case 44:
                return {
                    {0, 0, 170}
                };
            case 35:
            case 45:
                return {
                    {170, 0, 170}
                };
            case 36:
            case 46:
                return {
                    {0, 170, 170}
                };
            case 37:
            case 47:
                return {
                    {170, 170, 170}
                };
            case 90:
            case 100:
                return {
                    {85, 85, 85}
                };
            case 91:
            case 101:
                return {
                    {255, 85, 85}
                };
            case 92:
            case 102:
                return {
                    {85, 255, 85}
                };
            case 93:
            case 103:
                return {
                    {255, 255, 85}
                };
            case 94:
            case 104:
                return {
                    {85, 85, 255}
                };
            case 95:
            case 105:
                return {
                    {255, 85, 255}
                };
            case 96:
            case 106:
                return {
                    {85, 255, 255}
                };
            case 97:
            case 107:
                return {
                    {255, 255, 255}
                };
            case 38:
            case 48:
                return {toRGBFromMetadata()};
            default:
                return {};
        }
    }

    std::optional<rush::Vec3f> TextEffect::toRGBFromMetadata() const
    {
        if (metadata[0] == 5) {
            uint8_t code = metadata[1];
            if (code < 16) {
                if (code < 8) {
                    return TextEffect(30 + code).toRGB();
                }
                return TextEffect(90 + code - 8).toRGB();
            }

            if (code < 232) {
                code -= 16;
                uint32_t b = code % 6;
                uint32_t g = code / 6 % 6;
                uint32_t r = code / 36;
                return {
                    {r * 255 / 6, g * 255 / 6, b * 255 / 6}
                };
            }

            code -= 231;
            uint32_t gr = code * 255 / 24;
            return {
                {gr, gr, gr}
            };
        }
        if (metadata[0] == 2) {
            return {
                {metadata[1], metadata[2], metadata[3]}
            };
        }
        return {};
    }

    TextEffect TextEffect::reset()
    {
        return TextEffect(0);
    }

    TextEffect TextEffect::bold()
    {
        return TextEffect(1);
    }

    TextEffect TextEffect::faint()
    {
        return TextEffect(2);
    }

    TextEffect TextEffect::italic()
    {
        return TextEffect(3);
    }

    TextEffect TextEffect::underline()
    {
        return TextEffect(4);
    }

    TextEffect TextEffect::slowBlink()
    {
        return TextEffect(5);
    }

    TextEffect TextEffect::fastBlink()
    {
        return TextEffect(6);
    }

    TextEffect TextEffect::conceal()
    {
        return TextEffect(7);
    }

    TextEffect TextEffect::crossedOut()
    {
        return TextEffect(8);
    }

    TextEffect TextEffect::font(uint8_t font)
    {
        if (font > 9) {
            font = 0;
        }
        return TextEffect(10 + font);
    }

    TextEffect TextEffect::fraktrur()
    {
        return TextEffect(20);
    }

    TextEffect TextEffect::foreground4bits(uint8_t color)
    {
        color &= 0b1111;
        if (color < 8) {
            return TextEffect(30 + color);
        }
        return TextEffect(90 + (color & 0b111));
    }

    TextEffect TextEffect::foreground4bits(TextColor4Bits color)
    {
        return foreground4bits(static_cast<uint8_t>(color));
    }

    TextEffect TextEffect::background4bits(uint8_t color)
    {
        color &= 0b1111;
        if (color < 8) {
            return TextEffect(40 + color);
        }
        return TextEffect(100 + (color & 0b111));
    }

    TextEffect TextEffect::background4bits(TextColor4Bits color)
    {
        return background4bits(static_cast<uint8_t>(color));
    }

    TextEffect TextEffect::foreground8bits(uint8_t color)
    {
        return TextEffect(38, {5, color});
    }

    TextEffect TextEffect::background8bits(uint8_t color)
    {
        return TextEffect(48, {5, color});
    }

    TextEffect TextEffect::foregroundRGB(uint8_t red, uint8_t green, uint8_t blue)
    {
        return TextEffect(38, {2, red, green, blue});
    }

    TextEffect TextEffect::backgroundRGB(uint8_t red, uint8_t green, uint8_t blue)
    {
        return TextEffect(48, {2, red, green, blue});
    }

    TextEffect TextEffect::defaultForeground()
    {
        return TextEffect(39);
    }

    TextEffect TextEffect::defaultBackground()
    {
        return TextEffect(49);
    }

    TextEffect TextEffect::framed()
    {
        return TextEffect(51);
    }

    TextEffect TextEffect::encircled()
    {
        return TextEffect(52);
    }

    TextEffect TextEffect::overlined()
    {
        return TextEffect(53);
    }
} // namespace neon
