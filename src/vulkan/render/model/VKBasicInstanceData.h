//
// Created by gaelr on 9/15/2024.
//

#ifndef VKBASICINSTANCEDATA_H
#define VKBASICINSTANCEDATA_H


namespace neon {
    class CommandBuffer;
}

namespace neon::vulkan {
    class VKBasicInstanceData {
    public:
        void flush(const CommandBuffer& command);
    };
}

#endif //VKBASICINSTANCEDATA_H
