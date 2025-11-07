//
// Created by gaeqs on 7/11/25.
//

#ifndef NEON_MINECRAFTCOLORFORMATTER_H
#define NEON_MINECRAFTCOLORFORMATTER_H

#include <neon/logging/Message.h>
#include <neon/util/format/Formatter.h>

namespace neon
{

    struct MinecraftEffect
    {
        TextEffect effect;
        bool reset;
    };

    class MinecraftColorFormatter : public Formatter<std::string, SimpleMessage>
    {
        std::optional<std::unordered_map<char, MinecraftEffect>> _customEffectMap;

        const std::unordered_map<char, MinecraftEffect>& getEffectMap() const;

      public:
        MinecraftColorFormatter() = default;

        ~MinecraftColorFormatter() override = default;

        SimpleMessage format(const std::string& input) override;
    };
} // namespace neon

#endif // NEON_MINECRAFTCOLORFORMATTER_H
