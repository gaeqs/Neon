//
// Created by gaeqs on 16/04/2025.
//

#ifndef COMMANDBUFFERRUN_H
#define COMMANDBUFFERRUN_H

namespace neon
{

    class CommandBufferRun
    {
      public:
        virtual ~CommandBufferRun() = default;

        virtual bool hasFinished() = 0;
    };

} // namespace neon

#endif // COMMANDBUFFERRUN_H
