//
// Created by gaeqs on 8/09/24.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include <chrono>
#include <source_location>
#include <string>
#include <vector>
#include <sstream>

#include "TextEffect.h"

namespace neon
{
    class Logger;

    /**
     * Represents a part of a message.
     */
    struct MessagePart
    {
        std::vector<TextEffect> effects;
        std::string text;
    };

    /**
     * Represents a message that can be printed
     * by a Logger.
     * <p>
     * Messages contains the time point and the source location
     * where it was created.
     * It is recommended send MessageBuilders to loggers instead
     * of messages.
     * <p>
     * A message is split in several part, each one with several
     * styles that tells the output how to format the part.
     * <p>
     * To create a Message in an easier way, use the class MessageBuilder.
     */
    struct Message
    {
        std::chrono::system_clock::time_point timePoint;
        std::source_location sourceLocation;
        std::vector<std::string> groups;
        std::vector<MessagePart> parts;

        /**
         * Creates an empty message.
         */
        explicit Message(std::source_location location = std::source_location::current());

        /**
         * Creates a message containing only one part with the given string.
         * @param message the given string.
         */
        explicit Message(std::string message, std::source_location location = std::source_location::current());
    };

    /**
     * A struct similar to Message that has no groups, source location
     * nor time point.
     * <p>
     * You can use it to style prefixes, suffixes or other simple messages.
     */
    struct SimpleMessage
    {
        std::vector<MessagePart> parts;

        /**
         * Creates an empty simple message.
         */
        explicit SimpleMessage();

        /**
         * Creates a simple message containing only one part
         * with the given string.
         * @param message the given string.
         */
        explicit SimpleMessage(std::string message);
    };

    /**
     * Represents a group that can be used to tag messages.
     */
    struct MessageGroup
    {
        std::string name;
        SimpleMessage prefix;
    };

    /**
     * Helper class to build a Message.
     * <p>
     * This helper class implements a stack you can use to
     * push and pop text effects.
     * <p>
     * Use push() to create a new layer of effects.
     * Use effect() to add effects to the current layer.
     * Use pop() to delete the current layer.
     * <p>
     * Use print() or println() to add text to the message.
     * The text will be styled using the effect of all layers
     * in the stack.
     * <p>
     * Finally, use build() to build the final Message or
     * buildSimple() to build a SimpleMessage.
     */
    class MessageBuilder
    {
        std::vector<std::string> _groups;
        std::vector<MessagePart> _builtMessages;
        std::vector<std::vector<TextEffect>> _stack;
        size_t _effectAmount;
        Logger* _logger;
        std::source_location _loggerSourceLocation;

      public:
        MessageBuilder(const MessageBuilder& other);

        MessageBuilder& operator=(const MessageBuilder& other);

        MessageBuilder(MessageBuilder&& other) noexcept;

        MessageBuilder& operator=(MessageBuilder&& other) noexcept;

        MessageBuilder();

        MessageBuilder(Logger* logger, const std::source_location& loggerSourceLocation);

        ~MessageBuilder();

        MessageBuilder& group(std::string group);

        MessageBuilder& removeGroups();

        MessageBuilder& push();

        MessageBuilder& pop();

        MessageBuilder& effect(TextEffect effect);

        MessageBuilder& print(std::string message);

        MessageBuilder& print(std::string message, TextEffect effect);

        MessageBuilder& println(const std::string& message);

        MessageBuilder& println(const std::string& message, TextEffect effect);

        [[nodiscard]] Message build(std::source_location location = std::source_location::current()) const;

        [[nodiscard]] SimpleMessage buildSimple() const;

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageBuilder& print(const T& t)
        {
            std::stringstream ss;
            ss << t;
            return print(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageBuilder& print(const T& t, TextEffect effect)
        {
            std::stringstream ss;
            ss << t;
            return print(ss.str(), effect);
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageBuilder& println(const T& t)
        {
            std::stringstream ss;
            ss << t;
            return println(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageBuilder& println(const T& t, TextEffect effect)
        {
            std::stringstream ss;
            ss << t;
            return println(ss.str(), effect);
        }

        template<typename T>
        MessageBuilder& operator<<(const T& t)
        {
            if constexpr (std::is_same_v<TextEffect, T>) {
                effect(t);
                return *this;
            } else {
                return print(t);
            }
        }
    };

    /**
     * Helper class to create a MessageGroup.
     * <p>
     * This helper class implements a stack you can use to
     * push and pop text effects.
     * <p>
     * Use push() to create a new layer of effects.
     * Use effect() to add effects to the current layer.
     * Use pop() to delete the current layer.
     * <p>
     * Use print() or println() to add text to the message.
     * The text will be styled using the effect of all layers
     * in the stack.
     * <p>
     * Finally, use build() to build the final Message or
     * buildSimple() to build a SimpleMessage.
     */
    class MessageGroupBuilder
    {
        std::vector<MessagePart> _builtMessages;
        std::vector<std::vector<TextEffect>> _stack;
        size_t _effectAmount;

      public:
        MessageGroupBuilder();

        MessageGroupBuilder& push();

        MessageGroupBuilder& pop();

        MessageGroupBuilder& effect(TextEffect effect);

        MessageGroupBuilder& print(std::string message);

        MessageGroupBuilder& print(std::string message, TextEffect effect);

        MessageGroupBuilder& println(const std::string& message);

        MessageGroupBuilder& println(const std::string& message, TextEffect effect);

        [[nodiscard]] MessageGroup build(std::string name) const;

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageGroupBuilder& print(const T& t)
        {
            std::stringstream ss;
            ss << t;
            return print(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageGroupBuilder& print(const T& t, TextEffect effect)
        {
            std::stringstream ss;
            ss << t;
            return print(ss.str(), effect);
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageGroupBuilder& println(const T& t)
        {
            std::stringstream ss;
            ss << t;
            return println(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string>)
        MessageGroupBuilder& println(const T& t, TextEffect effect)
        {
            std::stringstream ss;
            ss << t;
            return println(ss.str(), effect);
        }
    };
} // namespace neon

#endif //MESSAGE_H
