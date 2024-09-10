//
// Created by gaeqs on 9/09/24.
//

#ifndef LOGCOMPONENT_H
#define LOGCOMPONENT_H

#include <mutex>
#include <neon/logging/Message.h>
#include <neon/structure/Component.h>

namespace neon {
    class LogComponent : public Component {
        std::vector<std::pair<Message, std::optional<MessageGroup>>> _messages;
        mutable std::mutex _mutex;
        SimpleMessage _locationMessage;

        uint64_t _outputId;

        bool printLocation(const std::source_location& location);

        void printPart(const MessagePart& part);

    public:
        ~LogComponent() override;

        void onStart() override;

        void onPreDraw() override;

        void addMessage(const Message& message,
                        const MessageGroup* group);
    };
}

#endif //LOGCOMPONENT_H
