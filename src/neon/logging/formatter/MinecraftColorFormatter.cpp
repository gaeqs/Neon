//
// Created by gaeqs on 7/11/25.
//

#include "MinecraftColorFormatter.h"

#include <string_view>

namespace
{
    std::unordered_map<char, neon::MinecraftEffect> _defaultEffectMap = {

        {'u',                 {neon::TextEffect::underline(), false}},
        {'l',                      {neon::TextEffect::bold(), false}},
        {'o',                    {neon::TextEffect::italic(), false}},
        {'m',                {neon::TextEffect::crossedOut(), false}},
        {'k',                   {neon::TextEffect::conceal(), false}},
        {'r',                     {neon::TextEffect::reset(), false}},
        {'0',       {neon::TextEffect::foregroundRGB(0, 0, 0), true}},
        {'1',     {neon::TextEffect::foregroundRGB(0, 0, 170), true}},
        {'2',     {neon::TextEffect::foregroundRGB(0, 170, 0), true}},
        {'3',   {neon::TextEffect::foregroundRGB(0, 170, 170), true}},
        {'4',     {neon::TextEffect::foregroundRGB(170, 0, 0), true}},
        {'5',   {neon::TextEffect::foregroundRGB(170, 0, 170), true}},
        {'6',   {neon::TextEffect::foregroundRGB(255, 170, 0), true}},
        {'7', {neon::TextEffect::foregroundRGB(170, 170, 170), true}},
        {'8',    {neon::TextEffect::foregroundRGB(85, 85, 85), true}},
        {'9',   {neon::TextEffect::foregroundRGB(85, 85, 255), true}},
        {'a',   {neon::TextEffect::foregroundRGB(85, 255, 85), true}},
        {'b',  {neon::TextEffect::foregroundRGB(85, 255, 255), true}},
        {'c',   {neon::TextEffect::foregroundRGB(255, 85, 85), true}},
        {'d',  {neon::TextEffect::foregroundRGB(255, 85, 255), true}},
        {'e',  {neon::TextEffect::foregroundRGB(255, 255, 85), true}},
        {'f', {neon::TextEffect::foregroundRGB(255, 255, 255), true}},
    };
}

namespace neon
{
    const std::unordered_map<char, MinecraftEffect>& MinecraftColorFormatter::getEffectMap() const
    {
        if (_customEffectMap.has_value()) {
            return _customEffectMap.value();
        }
        return _defaultEffectMap;
    }

    SimpleMessage MinecraftColorFormatter::format(const std::string& input)
    {
        MessageBuilder builder;
        size_t currentSegmentStart = 0;

        std::string_view view(input);

        for (size_t i = 0; i < view.length(); ++i) {
            if (char c = view[i]; c == '&' && i + 1 < view.length()) {
                char code = view[i + 1];
                if (code == '&') {
                    // Escape secuence
                    if (i != currentSegmentStart) {
                        builder << view.substr(currentSegmentStart, i - currentSegmentStart);
                    }
                    builder << '&';
                    currentSegmentStart = i + 2;
                    ++i;
                    continue;
                }

                auto& map = getEffectMap();
                if (auto it = map.find(code); it != map.end()) {
                    if (i != currentSegmentStart) {
                        builder << view.substr(currentSegmentStart, i - currentSegmentStart);
                    }
                    if (it->second.reset) {
                        builder.pop();
                    }

                    builder << it->second.effect;
                    currentSegmentStart = i + 2;
                    ++i;
                }
            }
        }

        if (currentSegmentStart < input.length()) {
            builder << input.substr(currentSegmentStart);
        }

        return builder.buildSimple();
    }
} // namespace neon