// LogComponent.h
// (User requested code in English)

#ifndef LOGCOMPONENT_H
#define LOGCOMPONENT_H

#include <mutex>
#include <vector>
#include <imgui.h>
#include <neon/logging/Message.h>
#include <neon/structure/Component.h>

namespace neon
{

    struct LogComponentMessage
    {
        Message message;
        std::vector<MessageGroup> groups;
    };

    class LogComponent : public Component
    {
        // Messages pending to be added to the main list.
        // This vector is protected by the mutex.
        std::vector<LogComponentMessage> _pendingMessages;

        // Messages displayed in the log. Only accessed by the ImGui thread.
        std::vector<LogComponentMessage> _messages;
        std::vector<size_t> _filteredMessages;

        mutable std::mutex _mutex;
        SimpleMessage _locationMessage;
        uint64_t _outputId = 0;

        ImGuiTextFilter _filter;
        bool _autoScroll = true;

        float _cachedGroupsMaxWidth = 0.0f;
        float _cachedLocationMaxWidth = 0.0f;

        float printLocation(const std::source_location& location);

        float printGroups(const LogComponentMessage* msg);

        void printPart(const MessagePart& part);

        void refreshFilteredMessages();

        void filterRecentMessages(size_t start);

      public:
        ~LogComponent() override;

        void onStart() override;
        void onPreDraw() override;

        /**
         * Adds a message to the pending queue.
         * This method is thread-safe.
         */
        void addMessage(const Message& message, const std::vector<const MessageGroup*>& groups);

        /**
         * Clears all messages from the log.
         * This method is thread-safe.
         */
        void clear();
    };

    REGISTER_COMPONENT(LogComponent, "Log Component")
} // namespace neon

#endif // LOGCOMPONENT_H