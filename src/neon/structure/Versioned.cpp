//
// Created by gaeqs on 28/05/2025.
//

#include "Versioned.h"

namespace neon
{
    Versioned::Versioned() :
        _version(0)
    {
    }

    uint64_t Versioned::getVersion() const
    {
        return _version;
    }

    void Versioned::incrementVersion()
    {
        ++_version;
    }
} // namespace neon