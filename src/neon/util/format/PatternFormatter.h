//
// Created by gaeqs on 6/11/25.
//

#ifndef NEON_PATTERNFORMATTER_H
#define NEON_PATTERNFORMATTER_H

#include <string>
#include <functional>
#include <unordered_map>
#include <sstream>

#include <neon/util/format/Formatter.h>

namespace neon
{

    template<typename Input>
    class PatternFormatter : public Formatter<Input, std::string>
    {
      public:
        using PlaceholderHandler = std::function<std::string(const Input&)>;

      private:
        std::string _pattern;
        std::unordered_map<std::string, PlaceholderHandler> _placeholders;

      public:
        explicit PatternFormatter(std::string pattern) :
            _pattern(std::move(pattern))
        {
        }

        ~PatternFormatter() override = default;

        void registerPlaceholder(std::string name, PlaceholderHandler handler)
        {
            _placeholders[std::move(name)] = std::move(handler);
        }

        std::string format(const Input& input) override
        {
            std::stringstream result;
            bool inPlaceholder = false;
            size_t placeholderStart = 0;

            for (size_t i = 0; i < _pattern.length(); ++i) {
                char c = _pattern[i];

                if (inPlaceholder) {
                    if (c == '}') {
                        inPlaceholder = false;

                        std::string_view key(_pattern.data() + placeholderStart, i - placeholderStart);

                        auto it = _placeholders.find(key);
                        if (it != _placeholders.end()) {
                            result << it->second(input);
                        } else {
                            // Key not found, append as literal
                            result << '{' << key << '}';
                        }
                    }
                } else { // Not in a placeholder
                    if (c == '{') {
                        if (i + 1 < _pattern.length() && _pattern[i + 1] == '{') {
                            result << '{';
                            ++i;
                        } else {
                            inPlaceholder = true;
                            placeholderStart = i + 1; // Key starts *after* the '{'
                        }
                    } else if (c == '}') {
                        if (i + 1 < _pattern.length() && _pattern[i + 1] == '}') {
                            result << '}';
                            ++i;
                        } else {
                            result << c; // Unmatched '}'
                        }
                    } else {
                        result << c;
                    }
                }
            }

            // Handle unclosed placeholder
            if (inPlaceholder) {
                result << '{'
                       << std::string_view(_pattern.data() + placeholderStart, _pattern.length() - placeholderStart);
            }

            return result.str();
        }
    };

} // namespace neon

#endif // NEON_PATTERNFORMATTER_H
