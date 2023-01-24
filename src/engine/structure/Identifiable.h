//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_IDENTIFIABLE_H
#define RVTRACKING_IDENTIFIABLE_H

#include <cstdint>

class Identifiable {
public:
    [[nodiscard]] virtual uint64_t getId() const = 0;
};

#endif //RVTRACKING_IDENTIFIABLE_H
