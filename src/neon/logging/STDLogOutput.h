//
// Created by gaeqs on 8/09/24.
//

#ifndef STDLOGOUTPUT_H
#define STDLOGOUTPUT_H

#include <source_location>

#include "LogOutput.h"
#include "Message.h"


namespace neon {
    class STDLogOutput : public LogOutput {
        SimpleMessage _locationMessage;

        void printLocation(const std::source_location& location);

        void printPart(const MessagePart& part);

    public:
        ~STDLogOutput() override;

        STDLogOutput();

        STDLogOutput(const STDLogOutput& other) = delete;

        void print(const Message& message,
                   const MessageGroup* group) override;
    };
}


#endif //STDLOGOUTPUT_H
