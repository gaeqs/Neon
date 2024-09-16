//
// Created by gaeqs on 16/09/2024.
//

#ifndef VKINSTANCEDATA_H
#define VKINSTANCEDATA_H

#include <vector>
#include <memory>

namespace neon::vulkan {
    class Buffer;

    class VKInstanceData {
    public:
        virtual ~VKInstanceData() = default;

        [[nodiscard]] virtual const std::vector<std::unique_ptr<Buffer>>&
        getBuffers() const = 0;
    };
}


#endif //VKINSTANCEDATA_H
