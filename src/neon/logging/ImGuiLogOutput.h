//
// Created by gaeqs on 9/09/24.
//

#ifndef IMGUILOGOUTPUT_H
#define IMGUILOGOUTPUT_H

#include <vector>
#include <neon/logging/LogOutput.h>
#include <neon/structure/IdentifiableWrapper.h>
#include <neon/util/component/LogComponent.h>

namespace neon {
    /**
     * Logger output used by a LogComponent.
     */
    class ImGuiLogOutput : public LogOutput {
        IdentifiableWrapper<LogComponent> _component;

    public:
        ImGuiLogOutput(IdentifiableWrapper<LogComponent> component);

        ~ImGuiLogOutput() override;

        void print(const Message& message,
                   const std::vector<const MessageGroup*>& groups) override;
    };
}


#endif //IMGUILOGOUTPUT_H
