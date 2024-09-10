//
// Created by gaeqs on 8/09/24.
//

#ifndef MESSAGE_H
#define MESSAGE_H
#include <chrono>
#include <source_location>
#include <stack>
#include <string>
#include <vector>

#include "TextEffect.h"

namespace neon {
    struct MessagePart {
        std::vector<TextEffect> effects;
        std::string text;
    };

    struct Message {
        std::chrono::system_clock::time_point timePoint;
        std::source_location sourceLocation;
        std::optional<std::string> group;
        std::vector<MessagePart> parts;

        explicit Message(std::source_location location =
                std::source_location::current());

        explicit Message(std::string message,
                         std::source_location location =
                                 std::source_location::current());
    };

    struct SimpleMessage {
        std::vector<MessagePart> parts;

        explicit SimpleMessage();

        explicit SimpleMessage(std::string message);
    };

    struct MessageGroup {
        std::string name;
        std::vector<MessagePart> prefix;
    };


    class MessageBuilder {
        std::optional<std::string> _group;
        std::vector<MessagePart> _builtMessages;
        std::vector<std::vector<TextEffect>> _stack;
        size_t _effectAmount;

    public:
        MessageBuilder();

        MessageBuilder& group(std::string group);

        MessageBuilder& removeGroup();

        MessageBuilder& push();

        MessageBuilder& pop();

        MessageBuilder& effect(TextEffect effect);

        MessageBuilder& print(std::string message);

        MessageBuilder& print(std::string message, TextEffect effect);

        MessageBuilder& println(const std::string& message);

        MessageBuilder& println(const std::string& message, TextEffect effect);

        [[nodiscard]] Message build(std::source_location location
                = std::source_location::current()) const;

        [[nodiscard]] SimpleMessage buildSimple() const;

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageBuilder& print(const T& t) {
            std::stringstream ss;
            ss << t;
            return print(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageBuilder& print(const T& t, TextEffect effect) {
            std::stringstream ss;
            ss << t;
            return print(ss.str(), effect);
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageBuilder& println(const T& t) {
            std::stringstream ss;
            ss << t;
            return println(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageBuilder& println(const T& t, TextEffect effect) {
            std::stringstream ss;
            ss << t;
            return println(ss.str(), effect);
        }
    };

    class MessageGroupBuilder {
        std::vector<MessagePart> _builtMessages;
        std::vector<std::vector<TextEffect>> _stack;
        size_t _effectAmount;

    public:
        MessageGroupBuilder();

        void push();

        bool pop();

        void effect(TextEffect effect);

        void print(std::string message);

        void print(std::string message, TextEffect effect);

        void println(const std::string& message);

        void println(const std::string& message, TextEffect effect);

        [[nodiscard]] MessageGroup build(std::string name) const;
    };
}


#endif //MESSAGE_H
