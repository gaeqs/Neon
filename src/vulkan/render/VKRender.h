//
// Created by gaelr on 22/12/2022.
//

#ifndef NEON_VKRENDER_H
#define NEON_VKRENDER_H

#include <vector>
#include <memory>

#include <neon/render/RenderPassStrategy.h>

namespace neon {
    class Application;

    class Room;

    class Material;
}

namespace neon::vulkan {

    class AbstractVKApplication;

    class VKRender {

        AbstractVKApplication* _vkApplication;
        mutable bool _drawImGui;

    public:

        VKRender(const VKRender& other) = delete;

        explicit VKRender(Application* application);

        void render(
                Room* room,
                const Render* render,
                const std::vector<std::shared_ptr<Material>>& sortedMaterials,
                const std::vector<std::shared_ptr<RenderPassStrategy>>& strategies) const;

        void beginRenderPass(const std::shared_ptr<FrameBuffer>& fb,
                             bool clear) const;

        void endRenderPass() const;

        void setupFrameBufferRecreation();

    };
}


#endif //NEON_VKRENDER_H
