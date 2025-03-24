//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_IDENTIFIABLE_H
#define RVTRACKING_IDENTIFIABLE_H

#include <cstdint>

namespace neon
{
    class Identifiable;

    struct IdentifiableCounter
    {
        uint32_t counter;
        bool valid;
        Identifiable* ptr;
    };

    class Identifiable
    {
        IdentifiableCounter* _counter;

      public:
        Identifiable();

        Identifiable(const Identifiable& other);

        Identifiable(Identifiable&& other) noexcept;

        virtual ~Identifiable();

        [[nodiscard]] IdentifiableCounter* getCounter() const;

        [[nodiscard]] virtual uint64_t getId() const = 0;
    };
} // namespace neon

#endif //RVTRACKING_IDENTIFIABLE_H
