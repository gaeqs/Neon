//
// Created by gaeqs on 8/09/24.
//

#include "Logger.h"

#include <utility>

#include "STDLogOutput.h"

namespace neon {
    void Logger::addDefaultGroups() {
        const std::string names[5] = {"info", "done", "warning", "error", "debug"};
        const std::string display[5] = {"INFO", "DONE", "WARNING", "ERROR", "DEBUG"};
        constexpr uint8_t color1[5] = {4, 2, 3, 1, 5};
        constexpr uint8_t color2[5] = {12, 10, 11, 9, 13};

        for (size_t i = 0; i < 5; ++i) {
            MessageGroupBuilder builder;
            builder.print("[", TextEffect::foreground4bits(color1[i]));
            builder.print(display[i], TextEffect::foreground4bits(color2[i]));
            builder.print(
                "] ", TextEffect::foreground4bits(color1[i]));
            addGroup(builder.build(names[i]));
        }
    }

    Logger::Logger(bool withDefaultGroups, bool withDefaultOutput) {
        if (withDefaultGroups) {
            addDefaultGroups();
        }
        if (withDefaultOutput) {
            addOutput(std::make_unique<STDLogOutput>());
        }
    }

    void Logger::addOutput(std::unique_ptr<LogOutput>&& output) {
        std::unique_lock lock(_mutex);
        _outputs.push_back(std::move(output));
    }

    const std::vector<std::unique_ptr<LogOutput>>& Logger::getOutputs() const {
        return _outputs;
    }

    void Logger::addGroup(MessageGroup&& group) {
        std::unique_lock lock(_mutex);
        _groups[group.name] = std::move(group);
    }

    void Logger::removeGroup(const std::string& group) {
        std::unique_lock lock(_mutex);
        _groups.erase(group);
    }

    void Logger::removeOutput(uint64_t id) {
        std::unique_lock lock(_mutex);
        std::erase_if(
            _outputs,
            [id](const std::unique_ptr<LogOutput>& output) {
                return output->getId() == id;
            }
        );
    }

    void Logger::print(const Message& message) const {
        std::unique_lock lock(_mutex);
        const MessageGroup* group = nullptr;
        if (message.group.has_value()) {
            auto it = _groups.find(message.group.value());
            if (it != _groups.end()) {
                group = &it->second;
            }
        }
        for (auto& output: _outputs) {
            output->print(message, group);
        }
    }

    void Logger::print(const MessageBuilder& message,
                       std::source_location location) const {
        print(message.build(location));
    }

    void Logger::print(std::string message,
                       std::source_location location) const {
        print(Message(std::move(message), location));
    }

    void Logger::info(std::string message,
                      std::source_location location) const {
        Message msg(std::move(message), location);
        msg.group = "info";
        print(msg);
    }

    void Logger::done(std::string message,
        std::source_location location) const {
        Message msg(std::move(message), location);
        msg.group = "done";
        print(msg);
    }

    void Logger::debug(std::string message,
                       std::source_location location) const {
        Message msg(std::move(message), location);
        msg.group = "debug";
        print(msg);
    }

    void Logger::warning(std::string message,
                         std::source_location location) const {
        Message msg(std::move(message), location);
        msg.group = "warning";
        print(msg);
    }

    void Logger::error(std::string message,
                       std::source_location location) const {
        Message msg(std::move(message), location);
        msg.group = "error";
        print(msg);
    }

    void Logger::info(Message message) const {
        message.group = "info";
        print(message);
    }

    void Logger::done(Message message) const {
        message.group = "done";
        print(message);
    }

    void Logger::debug(Message message) const {
        message.group = "debug";
        print(message);
    }

    void Logger::warning(Message message) const {
        message.group = "warning";
        print(message);
    }

    void Logger::error(Message message) const {
        message.group = "error";
        print(message);
    }

    void Logger::info(const MessageBuilder& message,
                      std::source_location location) const {
        Message msg = message.build(location);
        msg.group = "info";
        print(msg);
    }

    void Logger::done(const MessageBuilder& message,
        std::source_location location) const {
        Message msg = message.build(location);
        msg.group = "done";
        print(msg);
    }

    void Logger::debug(const MessageBuilder& message,
                       std::source_location location) const {
        Message msg = message.build(location);
        msg.group = "debug";
        print(msg);
    }

    void Logger::warning(const MessageBuilder& message,
                         std::source_location location) const {
        Message msg = message.build(location);
        msg.group = "warning";
        print(msg);
    }

    void Logger::error(const MessageBuilder& message,
                       std::source_location location) const {
        Message msg = message.build(location);
        msg.group = "errror";
        print(msg);
    }
}
