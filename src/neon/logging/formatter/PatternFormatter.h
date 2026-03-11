// #ifndef NEON_PATTERNFORMATTER_H
// #define NEON_PATTERNFORMATTER_H
//
// #include "Formatter.h"
// #include <string>
// #include <functional>
// #include <unordered_map>
//
// namespace neon {
//
//     /**
//      * @brief A formatter that creates log strings based on a string pattern.
//      */
//     class PatternFormatter : public Formatter {
//     public:
//         using PlaceholderHandler = std::function<std::string(const Log&)>;
//
//         /**
//          * @brief Constructs a PatternFormatter.
//          * @param pattern The pattern to use for formatting.
//          */
//         explicit PatternFormatter(std::string pattern);
//
//         /**
//          * @brief Formats a log event into a string based on the pattern.
//          * @param log The log event information.
//          * @return The formatted string.
//          */
//         std::string format(const Log& log) override;
//
//         /**
//          * @brief Registers a new placeholder or overrides an existing one.
//          * @param name The name of the placeholder (e.g., "timestamp").
//          * @param handler The function that will generate the string for the placeholder.
//          */
//         void registerPlaceholder(const std::string& name, PlaceholderHandler handler);
//
//     private:
//         void addDefaultPlaceholders();
//
//         std::string _pattern;
//         std::unordered_map<std::string, PlaceholderHandler> _placeholders;
//     };
//
// } // namespace neon
//
// #endif //NEON_PATTERNFORMATTER_H
