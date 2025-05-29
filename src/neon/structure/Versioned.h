//
// Created by gaeqs on 28/05/2025.
//

#ifndef VERSIONCONTROL_H
#define VERSIONCONTROL_H

#include <cstdint>

namespace neon
{
    /**
     * Provides versioning capabilities for derived objects.
     * Each object has a version number that increments with every update.
     * Remember that this versioning system is not automatic and doesn't track
     * deep modifications.
     * If you want to force a new version, you can use incrementVersion() manually.
     */
    class Versioned
    {
        uint64_t _version;

      public:

        /**
         * Constructs a Versioned object with an initial unique version number.
         */
        Versioned();

        /**
         * Retrieves the current version of the object.
         * @return Current version number.
         */
        [[nodiscard]] uint64_t getVersion() const;

        /**
         * Sets a new unique version number, typically after modification.
         */
        void incrementVersion();
    };
} // namespace neon

#endif // VERSIONCONTROL_H
