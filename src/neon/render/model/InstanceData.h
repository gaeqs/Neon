//
// Created by gaelr on 9/15/2024.
//

#ifndef INSTANCEDATA_H
#define INSTANCEDATA_H

#include <cstdint>
#include <string>

#include <neon/util/Result.h>

namespace neon {
    class InstanceData {
    public:
        struct Instance {
            uint32_t* id;
        };

        virtual ~InstanceData() = default;

        virtual Result<Instance, std::string> createInstance() = 0;

        virtual bool freeInstance(Instance id) = 0;

        [[nodiscard]] virtual size_t getInstanceAmount() const = 0;

        virtual bool uploadData(Instance id, size_t index, void* data) = 0;

        virtual void flush() = 0;

        virtual void flush(const CommandBuffer* commandBuffer) = 0;
    };
}

#endif //INSTANCEDATA_H
