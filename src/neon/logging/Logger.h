//
// Created by gaeqs on 8/09/24.
//

#ifndef LOGGER_H
#define LOGGER_H
#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>

#include <neon/logging/LogOutput.h>
#include <neon/logging/Message.h>

namespace neon {
    class Logger {
        std::vector<std::unique_ptr<LogOutput>> _outputs;
        std::unordered_map<std::string, MessageGroup> _groups;
        mutable std::mutex _mutex;

        void addDefaultGroups();

    public:
        Logger(const Logger& other) = delete;

        explicit Logger(bool withDefaultGroups = true,
                        bool withDefaultOutput = true);

        void addOutput(std::unique_ptr<LogOutput>&& output);

        [[nodiscard]] const std::vector<std::unique_ptr<LogOutput>>&
        getOutputs() const;

        void addGroup(MessageGroup&& group);

        void removeGroup(const std::string& group);

        void removeOutput(uint64_t id);

        void print(const Message& message) const;

        // region Utils

        void print(
            const MessageBuilder& message,
            std::source_location location = std::source_location::current()
        ) const;

        void print(
            std::string message,
            std::source_location location = std::source_location::current()
        ) const;

        void info(
            std::string message,
            std::source_location location = std::source_location::current()
        ) const;

        void done(
            std::string message,
            std::source_location location = std::source_location::current()
        ) const;

        void debug(
            std::string message,
            std::source_location location = std::source_location::current()
        ) const;

        void warning(
            std::string message,
            std::source_location location = std::source_location::current()
        ) const;

        void error(
            std::string message,
            std::source_location location = std::source_location::current()
        ) const;

        void info(Message message) const;

        void done(Message message) const;

        void debug(Message message) const;

        void warning(Message message) const;

        void error(Message message) const;

        void info(
            const MessageBuilder& message,
            std::source_location location = std::source_location::current()
        ) const;

        void done(
            const MessageBuilder& message,
            std::source_location location = std::source_location::current()
        ) const;

        void debug(
            const MessageBuilder& message,
            std::source_location location = std::source_location::current()
        ) const;

        void warning(
            const MessageBuilder& message,
            std::source_location location = std::source_location::current()
        ) const;

        void error(
            const MessageBuilder& message,
            std::source_location location = std::source_location::current()
        ) const;


        // endregion
    };
}

#endif //LOGGER_H
