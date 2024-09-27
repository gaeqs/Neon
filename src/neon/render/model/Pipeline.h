//
// Created by gaeqs on 27/09/24.
//

#ifndef MODELDRAWMODE_H
#define MODELDRAWMODE_H

#include <functional>
#include <rush/rush.h>

namespace neon {
    enum class PipelineType {
        VERTEX,
        MESH
    };

    struct Pipeline {
        PipelineType type = PipelineType::VERTEX;

        std::function<rush::Vec3i(size_t)> callsConfigurator =
                [](size_t instances) {
            return rush::Vec3i(instances, 1, 1);
        };
    };
}

#endif //MODELDRAWMODE_H
