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

    class LogOutput : public Identifiable {
        uint64_t _id;

    public:
        LogOutput(const LogOutput& other) = delete;

        LogOutput();

        [[nodiscard]] uint64_t getId() const override;

        virtual void print(const Message& message,
                           const std::vector<const MessageGroup*>& group) = 0;
    };
}

#endif //LOGOUTPUT_H
