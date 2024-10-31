//
// Created by gaeqs on 8/09/24.
//

#include "STDLogOutput.h"

#include <filesystem>
#include <ranges>

#include "Message.h"

namespace neon {
    void STDLogOutput::printLocation(const std::source_location& location) {
        SimpleMessage message = _locationMessage;


        auto path = std::filesystem::path(location.file_name());
        std::string file = path.filename().string();
        std::string line = std::to_string(location.line());

        for (auto part: _locationMessage.parts) {
            if (part.text == "{FILE}") {
                part.text = file;
            } else if (part.text == "{LINE}") {
                part.text = line;
            }
            printPart(part);
        }
    }

    void STDLogOutput::printPart(const MessagePart& part) {
        std::string s = "\033[";

        for (const auto& [code, metadata]: part.effects) {
            s += std::to_string(code) + ";";
            if (code == 38 || code == 48) {
                s += std::to_string(metadata[0]) + ";";
                if (metadata[0] == 2) {
                    s += std::to_string(metadata[1]) + ";";
                    s += std::to_string(metadata[2]) + ";";
                    s += std::to_string(metadata[3]) + ";";
                }
                if (metadata[0] == 5) {
                    s += std::to_string(metadata[1]) + ";";
                }
            }
        }

        if (s.back() == ';') {
            s.back() = 'm';
        } else {
            s += 'm';
        }
        std::cout  << part.text ; // Reset to default
        //std::cout << s << part.text << "\033[0m"; // Reset to default
    }

    STDLogOutput::~STDLogOutput() = default;

    STDLogOutput::STDLogOutput() {
        const TextEffect textEffect = TextEffect::foregroundRGB(
            0xFF, 0x88, 0x88);

        MessageBuilder builder;
        builder.push();
        builder.effect(TextEffect::foregroundRGB(0xFF, 0x00, 0x00));
        builder.effect(TextEffect::bold());
        builder.print("[");
        builder.print("{FILE}", textEffect);
        builder.print(":");
        builder.print("{LINE}", textEffect);
        builder.print("] ");
        _locationMessage = builder.buildSimple();
    };

    void STDLogOutput::print(const Message& message,
                             const std::vector<const MessageGroup*>& groups) {
        printLocation(message.sourceLocation);
        for (auto* group: groups | std::ranges::views::reverse) {
            for (auto& part: group->prefix.parts) {
                printPart(part);
            }
        }
        if (!groups.empty()) {
            std::cout << " ";
        }
        for (const auto& part: message.parts) {
            printPart(part);
        }
        std::cout << std::endl;
    }
}
