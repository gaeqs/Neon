//
// Created by gaeqs on 8/09/24.
//

#ifndef LOGOUTPUT_H
#define LOGOUTPUT_H

#include <vector>
#include <neon/structure/Identifiable.h>


namespace neon {
    struct Message;
    struct MessageGroup;

    /**
     * Represents the output of a Logger instance.
     * You can create extensions of this function
     * to implement new log outputs.
     */
    class LogOutput : public Identifiable {
        uint64_t _id;

    public:
        LogOutput(const LogOutput& other) = delete;

        /**
         * Creates a new log output.
         */
        LogOutput();

        /**
         * @return the unique identifier of this log output.
         */
        [[nodiscard]] uint64_t getId() const override;

        /**
         * Prints the given message using the given groups.
         * <p>
         * This function WON'T be called by several threads
         * at the same time.
         * @param message the message.
         * @param group the group.
         */
        virtual void print(const Message& message,
                           const std::vector<const MessageGroup*>& group) = 0;
    };
}

#endif //LOGOUTPUT_H
