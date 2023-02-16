//
// Created by gaelr on 21/01/2023.
//

#ifndef NEON_IMGUITEST_H
#define NEON_IMGUITEST_H

#include <engine/structure/Component.h>
#include <engine/render/SimpleFrameBuffer.h>
#include <imgui.h>

namespace neon {
    class ViewportComponent : public Component {

        std::shared_ptr<SimpleFrameBuffer> _frameBuffer;
        ImVec2 _windowSize;

    public:

        void onStart() override;

        void onPreDraw() override;

    };
    REGISTER_COMPONENT(ViewportComponent, "Viewport")
}

#endif //NEON_IMGUITEST_H
