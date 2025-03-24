//
// Created by gaeqs on 8/09/24.
//

#include "LogOutput.h"

namespace neon
{
    uint64_t LOG_OUTPUT_ID_GENERATOR = 1;

    LogOutput::LogOutput() :
        _id(LOG_OUTPUT_ID_GENERATOR++)
    {
    }

    uint64_t LogOutput::getId() const
    {
        return _id;
    }
} // namespace neon
