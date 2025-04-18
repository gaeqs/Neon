//
// Created by gaelr on 21/01/2023.
//

#include "ViewportComponent.h"

#include <imgui_internal.h>
#include <neon/structure/Room.h>

namespace neon
{
    ViewportComponent::ViewportComponent() :
        _hovered(false)
    {
    }

    ViewportComponent::ViewportComponent(const std::shared_ptr<SimpleFrameBuffer>& frameBuffer) :
        _frameBuffer(frameBuffer),
        _hovered(false)
    {
    }

    void ViewportComponent::onStart()
    {
        _hovered = false;

        if (_frameBuffer != nullptr) {
            return;
        }
        auto& r = getRoom()->getApplication()->getRender();
        auto strategy = r->getStrategies()[r->getStrategyAmount() - 2];
        auto defaultStrategy = std::dynamic_pointer_cast<DefaultRenderPassStrategy>(strategy);

        if (defaultStrategy != nullptr) {
            _frameBuffer = std::dynamic_pointer_cast<SimpleFrameBuffer>(defaultStrategy->getFrameBuffer());
        }
    }

    void ViewportComponent::onPreDraw()
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(100000, 100000));
        if (ImGui::Begin("Viewport")) {
            _windowSize = ImGui::GetContentRegionAvail();
            _windowOrigin = ImGui::GetCursorScreenPos();
            getApplication()->forceViewport({_windowSize.x, _windowSize.y});
            if (_frameBuffer) {
                ImGui::Image(_frameBuffer->getImGuiDescriptor(0), _windowSize);
                _hovered = ImGui::IsItemHovered();
            }
        }
        ImGui::End();

        if (_windowSize.x > 0 && _windowSize.y > 0) {
            auto& c = getRoom()->getCamera();
            c.setFrustum(c.getFrustum().withAspectRatio(_windowSize.x / _windowSize.y));
        }
    }

    bool ViewportComponent::isHovered() const
    {
        return _hovered;
    }

    ImVec2 ViewportComponent::getWindowSize() const
    {
        return _windowSize;
    }

    ImVec2 ViewportComponent::getWindowOrigin() const
    {
        return _windowOrigin;
    }
} // namespace neon
