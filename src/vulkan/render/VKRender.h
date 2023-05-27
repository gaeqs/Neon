//
// Created by gaelr on 22/12/2022.
//

#ifndef NEON_VKRENDER_H
#define NEON_VKRENDER_H

#include <vector>
#include <memory>

#include <engine/render/RenderPassStrategy.h>
#include <util/ClusteredLinkedCollection.h>

namespace neon {
    class Application;

    class Room;

    class Material;
}

namespace neon::vulkan {

    class VKApplication;

    class VKRender {

        VKApplication* _vkApplication;

    public:

        VKRender(const VKRender& other) = delete;

        explicit VKRender(Application* application);

        void render(
                Room* room,
                const Render* render,
                const std::vector<std::shared_ptr<Material>>& sortedMaterials,
                const std::vector<std::shared_ptr<RenderPassStrategy>>& strategies) const;

        void beginRenderPass(const std::shared_ptr<FrameBuffer>& fb) const;

        void endRenderPass() const;

        void setupFrameBufferRecreation();

    };
}


#endif //NEON_VKRENDER_H
