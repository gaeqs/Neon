//
// Created by gaeqs on 8/09/24.
//

#include "Logger.h"

#include <utility>

#include "STDLogOutput.h"

namespace neon {
    namespace {
        Logger DEFAULT_LOGGER(true, true);
    }

    Logger& logger = DEFAULT_LOGGER;

    MessageBuilder log(const std::source_location& location) {
        return MessageBuilder(&DEFAULT_LOGGER, location);
    }

    MessageBuilder info(const std::source_location& location) {
        MessageBuilder b(&DEFAULT_LOGGER, location);
        b.group("info");
        return std::move(b);
    }

    MessageBuilder done(const std::source_location& location) {
        MessageBuilder b(&DEFAULT_LOGGER, location);
        b.group("done");
        return std::move(b);
    }

    MessageBuilder warning(const std::source_location& location) {
        MessageBuilder b(&DEFAULT_LOGGER, location);
        b.group("warning");
        return std::move(b);
    }

    MessageBuilder error(const std::source_location& location) {
        MessageBuilder b(&DEFAULT_LOGGER, location);
        b.group("error");
        return std::move(b);
    }

    MessageBuilder debug(const std::source_location& location) {
        MessageBuilder b(&DEFAULT_LOGGER, location);
        b.group("debug");
        return std::move(b);
    }

    void Logger::addDefaultGroups() {
        const std::string names[5] = {
            "info", "done", "warning", "error", "debug"
        };
        const std::string display[5] = {
            "INFO", "DONE", "WARNING", "ERROR", "DEBUG"
        };
        constexpr uint8_t color1[5] = {4, 2, 3, 1, 5};
        constexpr uint8_t color2[5] = {12, 10, 11, 9, 13};

        for (size_t i = 0; i < 5; ++i) {
            MessageGroupBuilder builder;
            builder.print("[", TextEffect::foreground4bits(color1[i]));
            builder.print(display[i], TextEffect::foreground4bits(color2[i]));
            builder.print(
                "]", TextEffect::foreground4bits(color1[i]));
            addGroup(builder.build(names[i]));
        }
    }

    Logger::Logger(bool withDefaultGroups,
                   bool withDefaultOutput) {
        if (withDefaultGroups) {
            addDefaultGroups();
        }
        if (withDefaultOutput) {
            addOutput(std::make_unique<STDLogOutput>());
        }
    }

    void Logger::addOutput(std::unique_ptr<LogOutput>&& output) {
        std::lock_guard lock(_mutex);
        _outputs.push_back(std::move(output));
    }

    const std::vector<std::unique_ptr<LogOutput>>& Logger::getOutputs() const {
        return _outputs;
    }

    void Logger::addGroup(MessageGroup&& group) {
        std::lock_guard lock(_mutex);
        _groups[group.name] = std::move(group);
    }

    bool Logger::removeGroup(const std::string& group) {
        std::lock_guard lock(_mutex);
        return _groups.erase(group) > 0;
    }

    bool Logger::removeOutput(uint64_t id) {
        std::lock_guard lock(_mutex);
        return std::erase_if(
                   _outputs,
                   [id](const std::unique_ptr<LogOutput>& output) {
                       return output->getId() == id;
                   }
               ) > 0;
    }

    void Logger::print(const Message& message) const {
        std::lock_guard lock(_mutex);

        std::vector<const MessageGroup*> groups;
        for (auto& name: message.groups) {
            auto it = _groups.find(name);
            if (it != _groups.end()) {
                groups.push_back(&it->second);
            }
        }

        for (auto& output: _outputs) {
            output->print(message, groups);
        }
    }

    void Logger::print(const MessageBuilder& message, std::source_location location) const {
        print(message.build(location));
    }

    void Logger::print(std::string message, std::source_location location) const {
        print(Message(std::move(message), location));
    }

    void Logger::info(std::string message, std::source_location location) const {
        Message msg(std::move(message), location);
        msg.groups.emplace_back("info");
        print(msg);
    }

    void Logger::done(std::string message, std::source_location location) const {
        Message msg(std::move(message), location);
        msg.groups.emplace_back("done");
        print(msg);
    }

    void Logger::debug(std::string message, std::source_location location) const {
        Message msg(std::move(message), location);
        msg.groups.emplace_back("debug");
        print(msg);
    }

    void Logger::warning(std::string message, std::source_location location) const {
        Message msg(std::move(message), location);
        msg.groups.emplace_back("warning");
        print(msg);
    }

    void Logger::error(std::string message, std::source_location location) const {
        Message msg(std::move(message), location);
        msg.groups.emplace_back("error");
        print(msg);
    }

    void Logger::info(Message message) const {
        message.groups.emplace_back("info");
        print(message);
    }

    void Logger::done(Message message) const {
        message.groups.emplace_back("done");
        print(message);
    }

    void Logger::debug(Message message) const {
        message.groups.emplace_back("debug");
        print(message);
    }

    void Logger::warning(Message message) const {
        message.groups.emplace_back("warning");
        print(message);
    }

    void Logger::error(Message message) const {
        message.groups.emplace_back("error");
        print(message);
    }

    void Logger::info(const MessageBuilder& message, std::source_location location) const {
        Message msg = message.build(location);
        msg.groups.emplace_back("info");
        print(msg);
    }

    void Logger::done(const MessageBuilder& message, std::source_location location) const {
        Message msg = message.build(location);
        msg.groups.emplace_back("done");
        print(msg);
    }

    void Logger::debug(const MessageBuilder& message, std::source_location location) const {
        Message msg = message.build(location);
        msg.groups.emplace_back("debug");
        print(msg);
    }

    void Logger::warning(const MessageBuilder& message, std::source_location location) const {
        Message msg = message.build(location);
        msg.groups.emplace_back("warning");
        print(msg);
    }

    void Logger::error(const MessageBuilder& message, std::source_location location) const {
        Message msg = message.build(location);
        msg.groups.emplace_back("error");
        print(msg);
    }
}
