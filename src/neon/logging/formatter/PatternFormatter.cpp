// #include "PatternFormatter.h"
// #include <chrono>
// #include <sstream>
// #include <filesystem>
// #include <iomanip>
//
// namespace neon {
//
//     // Helper to convert GroupLevel to string
//     std::string levelToString(GroupLevel level) {
//         switch (level) {
//             case GroupLevel::LOW: return "LOW";
//             case GroupLevel::NORMAL: return "NORMAL";
//             case GroupLevel::HIGH: return "HIGH";
//             case GroupLevel::URGENT: return "URGENT";
//             default: return "UNKNOWN";
//         }
//     }
//
//     PatternFormatter::PatternFormatter(std::string pattern)
//         : _pattern(std::move(pattern)) {
//         addDefaultPlaceholders();
//     }
//
//     void PatternFormatter::addDefaultPlaceholders() {
//         registerPlaceholder("timestamp", [](const Log& log) {
//             auto time_t = std::chrono::system_clock::to_time_t(log.message.timePoint);
//             auto tm = *std::localtime(&time_t);
//             std::stringstream ss;
//             ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
//             return ss.str();
//         });
//
//         registerPlaceholder("level", [](const Log& log) {
//             return levelToString(log.level);
//         });
//
//         registerPlaceholder("message", [](const Log& log) {
//             std::stringstream ss;
//             for(const auto& part : log.message.parts) {
//                 ss << part.text;
//             }
//             return ss.str();
//         });
//
//         registerPlaceholder("file", [](const Log& log) {
//             return std::filesystem::path(log.message.sourceLocation.file_name()).filename().string();
//         });
//
//         registerPlaceholder("line", [](const Log& log) {
//             return std::to_string(log.message.sourceLocation.line());
//         });
//
//         registerPlaceholder("groups", [](const Log& log) {
//             std::stringstream ss;
//             bool first = true;
//             for(const auto* group : log.groups) {
//                 if (!first) ss << ", ";
//                 ss << group->name;
//                 first = false;
//             }
//             return ss.str();
//         });
//     }
//
//     void PatternFormatter::registerPlaceholder(const std::string& name, PlaceholderHandler handler) {
//         _placeholders[name] = std::move(handler);
//     }
//
//     std::string PatternFormatter::format(const Log& log) {
//         std::stringstream result;
//         std::string placeholder;
//         bool inPlaceholder = false;
//
//         for (char c : _pattern) {
//             if (c == '{') {
//                 inPlaceholder = true;
//                 placeholder.clear();
//             } else if (c == '}' && inPlaceholder) {
//                 inPlaceholder = false;
//                 auto it = _placeholders.find(placeholder);
//                 if (it != _placeholders.end()) {
//                     result << it->second(log);
//                 } else {
//                     result << '{' << placeholder << '}';
//                 }
//             } else if (inPlaceholder) {
//                 placeholder += c;
//             } else {
//                 result << c;
//             }
//         }
//
//         return result.str();
//     }
//
// } // namespace neon
