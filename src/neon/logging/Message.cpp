//
// Created by gaeqs on 8/09/24.
//

#include "Message.h"

#include <neon/logging/Logger.h>

namespace neon
{
    Message::Message(std::source_location location) :
        timePoint(std::chrono::system_clock::now()),
        sourceLocation(location)
    {
    }

    Message::Message(std::string message, std::source_location location) :
        timePoint(std::chrono::system_clock::now()),
        sourceLocation(location)
    {
        MessagePart part;
        part.text = std::move(message);
        parts.push_back(std::move(part));
    }

    SimpleMessage::SimpleMessage() = default;

    SimpleMessage::SimpleMessage(std::string message)
    {
        MessagePart part;
        part.text = std::move(message);
        parts.push_back(std::move(part));
    }

    MessageBuilder::MessageBuilder(const MessageBuilder& other) :
        _groups(other._groups),
        _builtMessages(other._builtMessages),
        _stack(other._stack),
        _effectAmount(other._effectAmount),
        _logger(other._logger),
        _loggerSourceLocation(other._loggerSourceLocation)
    {
    }

    MessageBuilder& MessageBuilder::operator=(const MessageBuilder& other)
    {
        if (this == &other) {
            return *this;
        }
        _groups = other._groups;
        _builtMessages = other._builtMessages;
        _stack = other._stack;
        _effectAmount = other._effectAmount;
        _logger = other._logger;
        _loggerSourceLocation = other._loggerSourceLocation;
        return *this;
    }

    MessageBuilder::MessageBuilder(MessageBuilder&& other) noexcept :
        _groups(std::move(other._groups)),
        _builtMessages(std::move(other._builtMessages)),
        _stack(std::move(other._stack)),
        _effectAmount(other._effectAmount),
        _logger(other._logger),
        _loggerSourceLocation(other._loggerSourceLocation)
    {
        other._logger = nullptr;
    }

    MessageBuilder& MessageBuilder::operator=(MessageBuilder&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }
        _groups = std::move(other._groups);
        _builtMessages = std::move(other._builtMessages);
        _stack = std::move(other._stack);
        _effectAmount = other._effectAmount;
        _logger = other._logger;
        _loggerSourceLocation = other._loggerSourceLocation;
        other._logger = nullptr;
        return *this;
    }

    MessageBuilder::MessageBuilder() :
        _effectAmount(0),
        _logger(nullptr)
    {
        _stack.emplace_back();
    }

    MessageBuilder::MessageBuilder(Logger* logger, const std::source_location& loggerSourceLocation) :
        _effectAmount(0),
        _logger(logger),
        _loggerSourceLocation(loggerSourceLocation)
    {
        _stack.emplace_back();
    }

    MessageBuilder::~MessageBuilder()
    {
        if (_logger != nullptr) {
            _logger->print(*this, _loggerSourceLocation);
        }
    }

    MessageBuilder& MessageBuilder::group(std::string group)
    {
        _groups.push_back(std::move(group));
        return *this;
    }

    MessageBuilder& MessageBuilder::removeGroups()
    {
        _groups.clear();
        return *this;
    }

    MessageBuilder& MessageBuilder::push()
    {
        _stack.emplace_back();
        return *this;
    }

    MessageBuilder& MessageBuilder::pop()
    {
        if (_stack.size() < 2) {
            _stack[0].clear();
            _effectAmount = 0;
            return *this;
        }
        _effectAmount -= _stack.back().size();
        _stack.pop_back();
        return *this;
    }

    MessageBuilder& MessageBuilder::effect(TextEffect effect)
    {
        _stack.back().push_back(effect);
        ++_effectAmount;
        return *this;
    }

    MessageBuilder& MessageBuilder::print(std::string message)
    {
        MessagePart part;
        part.text = std::move(message);
        part.effects.reserve(_effectAmount);
        for (const auto& effects : _stack) {
            part.effects.insert(part.effects.end(), effects.begin(), effects.end());
        }
        _builtMessages.push_back(std::move(part));
        return *this;
    }

    MessageBuilder& MessageBuilder::print(std::string message, TextEffect effect)
    {
        MessagePart part;
        part.text = std::move(message);
        part.effects.reserve(_effectAmount);
        for (const auto& effects : _stack) {
            part.effects.insert(part.effects.end(), effects.begin(), effects.end());
            part.effects.push_back(effect);
        }
        _builtMessages.push_back(std::move(part));
        return *this;
    }

    MessageBuilder& MessageBuilder::println(const std::string& message)
    {
        return print(message + "\n");
    }

    MessageBuilder& MessageBuilder::println(const std::string& message, TextEffect effect)
    {
        return print(message + "\n", effect);
    }

    Message MessageBuilder::build(std::source_location location) const
    {
        Message message(location);
        message.parts = _builtMessages;
        message.timePoint = std::chrono::system_clock::now();
        message.groups = _groups;
        return message;
    }

    SimpleMessage MessageBuilder::buildSimple() const
    {
        SimpleMessage message;
        message.parts = _builtMessages;
        return message;
    }

    MessageGroupBuilder::MessageGroupBuilder() :
        _effectAmount(0)
    {
        _stack.emplace_back();
    }

    MessageGroupBuilder& MessageGroupBuilder::push()
    {
        _stack.emplace_back();
        return *this;
    }

    MessageGroupBuilder& MessageGroupBuilder::pop()
    {
        if (_stack.size() < 2) {
            _stack[0].clear();
            _effectAmount = 0;
        }
        _effectAmount -= _stack.back().size();
        _stack.pop_back();
        return *this;
    }

    MessageGroupBuilder& MessageGroupBuilder::effect(TextEffect effect)
    {
        _stack.back().push_back(effect);
        ++_effectAmount;
        return *this;
    }

    MessageGroupBuilder& MessageGroupBuilder::print(std::string message)
    {
        MessagePart part;
        part.text = std::move(message);
        part.effects.reserve(_effectAmount);
        for (const auto& effects : _stack) {
            part.effects.insert(part.effects.end(), effects.begin(), effects.end());
        }
        _builtMessages.push_back(std::move(part));
        return *this;
    }

    MessageGroupBuilder& MessageGroupBuilder::print(std::string message, TextEffect effect)
    {
        MessagePart part;
        part.text = std::move(message);
        part.effects.reserve(_effectAmount);
        for (const auto& effects : _stack) {
            part.effects.insert(part.effects.end(), effects.begin(), effects.end());
            part.effects.push_back(effect);
        }
        _builtMessages.push_back(std::move(part));
        return *this;
    }

    MessageGroupBuilder& MessageGroupBuilder::println(const std::string& message)
    {
        print(message + "\n");
        return *this;
    }

    MessageGroupBuilder& MessageGroupBuilder::println(const std::string& message, TextEffect effect)
    {
        print(message + "\n", effect);
        return *this;
    }

    MessageGroup MessageGroupBuilder::build(std::string name) const
    {
        MessageGroup group;
        group.name = std::move(name);
        group.prefix.parts = _builtMessages;
        return group;
    }
} // namespace neon
