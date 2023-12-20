//
// Created by gaeqs on 01/12/2023.
//

#ifndef VKCOMMANDPOOL_H
#define VKCOMMANDPOOL_H

namespace neon {
    class Application;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKCommandPool {
        AbstractVKApplication* _vkApplication;

    public:
        explicit VKCommandPool(Application* application);
    };
}


#endif //VKCOMMANDPOOL_H
