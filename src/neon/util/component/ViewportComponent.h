//
// Created by gaelr on 21/01/2023.
//

#ifndef NEON_IMGUITEST_H
#define NEON_IMGUITEST_H

#include <neon/structure/Component.h>
#include <neon/render/buffer/SimpleFrameBuffer.h>
#include <neon/render/texture/SampledTexture.h>
#include <imgui.h>

namespace neon
{
    class ViewportComponent : public Component
    {
        std::shared_ptr<SimpleFrameBuffer> _frameBuffer;
        std::shared_ptr<SampledTexture> _sampled;

        std::string _name;

        ImVec2 _windowSize;
        ImVec2 _windowOrigin;

        bool _hovered;

      public:
        ViewportComponent(const std::shared_ptr<SimpleFrameBuffer>& frameBuffer, std::string name = "Viewport");

        void onStart() override;

        void onPreDraw() override;

        [[nodiscard]] bool isHovered() const;

        [[nodiscard]] ImVec2 getWindowSize() const;

        [[nodiscard]] ImVec2 getWindowOrigin() const;
    };

    REGISTER_COMPONENT(ViewportComponent, "Viewport")
} // namespace neon

#endif // NEON_IMGUITEST_H
