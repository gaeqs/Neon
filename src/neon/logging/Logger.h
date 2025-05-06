//
// Created by gaeqs on 8/09/24.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <sstream>

#include <neon/logging/LogOutput.h>
#include <neon/logging/Message.h>

namespace neon
{
    /**
     * The main class of the logging system.
     * This class allows the use to print custom messages,
     * propagating them to the registered outputs.
     */
    class Logger
    {
        std::vector<std::unique_ptr<LogOutput>> _outputs;
        std::unordered_map<std::string, MessageGroup> _groups;
        mutable std::mutex _mutex;

        void addDefaultGroups();

      public:
        Logger(const Logger& other) = delete;

        /**
         * Creates a new logger.
         * @param withDefaultGroups whether the logger should include the
         * groups "info", "done", "warning", "error" and "debug" by default.
         * @param withDefaultOutput whether the logger should include a
         * STDLogOutput by default.
         * @param defaultLogger whether this logger should be considered
         * the default logger. If true, you can access this logger using
         * Logger::defaultLogger().
         */
        explicit Logger(bool withDefaultGroups = true, bool withDefaultOutput = true);

        /**
         * Adds a new log output.
         * Log outputs cannot be shared by loggers.
         * @param output the new log output.
         */
        void addOutput(std::unique_ptr<LogOutput>&& output);

        /**
         * @return the list of outputs inside this logger.
         */
        [[nodiscard]] const std::vector<std::unique_ptr<LogOutput>>& getOutputs() const;

        /**
         * Removes the output that matches the given id.
         * @param id the id.
         * @return whether any output has been removed.
         */
        bool removeOutput(uint64_t id);

        /**
         * Adds a new log group.
         * @param group the new log groups.
         */
        void addGroup(MessageGroup&& group);

        /**
         * Removes the log group that matches the given name.
         * @param group the name of the group.
         * @return whether any group has been removed.
         */
        bool removeGroup(const std::string& group);

        /**
         * Prints the given message.
         * @param message the message.
         */
        void print(const Message& message) const;

        // region Utils

        /**
         * Build and prints the given message builder.
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void print(const MessageBuilder& message,
                   std::source_location location = std::source_location::current()) const;

        /**
         * Prints the given message.
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void print(std::string message, std::source_location location = std::source_location::current()) const;

        /**
         * Prints the given message using the group log "info".
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void info(std::string message, std::source_location location = std::source_location::current()) const;

        /**
         * Prints the given message using the group log "done".
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void done(std::string message, std::source_location location = std::source_location::current()) const;

        /**
         * Prints the given message using the group log "debug".
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void debug(std::string message, std::source_location location = std::source_location::current()) const;

        /**
         * Prints the given message using the group log "warning".
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void warning(std::string message, std::source_location location = std::source_location::current()) const;

        /**
         * Prints the given message using the group log "error".
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void error(std::string message, std::source_location location = std::source_location::current()) const;

        /**
         * Prints the given message using the group log "info".
         * This method won't remove any group already present
         * in the message.
         * @param message the message.
         */
        void info(Message message) const;

        /**
         * Prints the given message using the group log "done".
         * This method won't remove any group already present
         * in the message.
         * @param message the message.
         */
        void done(Message message) const;

        /**
         * Prints the given message using the group log "debug".
         * This method won't remove any group already present
         * in the message.
         * @param message the message.
         */
        void debug(Message message) const;

        /**
         * Prints the given message using the group log "warning".
         * This method won't remove any group already present
         * in the message.
         * @param message the message.
         */
        void warning(Message message) const;

        /**
         * Prints the given message using the group log "error".
         * This method won't remove any group already present
         * in the message.
         * @param message the message.
         */
        void error(Message message) const;

        /**
         * Build and prints the given message
         * builder using the group log "info".
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void info(const MessageBuilder& message, std::source_location location = std::source_location::current()) const;

        /**
         * Build and prints the given message
         * builder using the group log "done".
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void done(const MessageBuilder& message, std::source_location location = std::source_location::current()) const;

        /**
         * Build and prints the given message
         * builder using the group log "debug".
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void debug(const MessageBuilder& message,
                   std::source_location location = std::source_location::current()) const;

        /**
         * Build and prints the given message
         * builder using the group log "warning".
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void warning(const MessageBuilder& message,
                     std::source_location location = std::source_location::current()) const;

        /**
         * Build and prints the given message
         * builder using the group log "error".
         * @param message the message.
         * @param location the location where this function has been called.
         */
        void error(const MessageBuilder& message,
                   std::source_location location = std::source_location::current()) const;

        template<typename T>
            requires(!std::is_convertible_v<T, std::string> && !std::is_same_v<T, Message> &&
                     !std::is_same_v<T, MessageBuilder>)
        void print(const T& t)
        {
            std::stringstream ss;
            ss << t;
            print(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string> && !std::is_same_v<T, Message> &&
                     !std::is_same_v<T, MessageBuilder>)
        void info(const T& t)
        {
            std::stringstream ss;
            ss << t;
            info(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string> && !std::is_same_v<T, Message> &&
                     !std::is_same_v<T, MessageBuilder>)
        void done(const T& t)
        {
            std::stringstream ss;
            ss << t;
            done(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string> && !std::is_same_v<T, Message> &&
                     !std::is_same_v<T, MessageBuilder>)
        void debug(const T& t)
        {
            std::stringstream ss;
            ss << t;
            debug(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string> && !std::is_same_v<T, Message> &&
                     !std::is_same_v<T, MessageBuilder>)
        void warning(const T& t)
        {
            std::stringstream ss;
            ss << t;
            warning(ss.str());
        }

        template<typename T>
            requires(!std::is_convertible_v<T, std::string> && !std::is_same_v<T, Message> &&
                     !std::is_same_v<T, MessageBuilder>)
        void error(const T& t)
        {
            std::stringstream ss;
            ss << t;
            error(ss.str());
        }

        // endregion
    };

    extern Logger& logger;

    MessageBuilder log(const std::source_location& location = std::source_location::current());

    MessageBuilder info(const std::source_location& location = std::source_location::current());

    MessageBuilder done(const std::source_location& location = std::source_location::current());

    MessageBuilder warning(const std::source_location& location = std::source_location::current());

    MessageBuilder error(const std::source_location& location = std::source_location::current());

    MessageBuilder debug(const std::source_location& location = std::source_location::current());
} // namespace neon

#endif //LOGGER_H
