//
// Created by gaeqs on 18/05/25.
//

#ifdef USE_CEF

    #include "CefWebView.h"

    #include <imgui.h>
    #include <imgui_demo.cpp>
    #include <neon/io/CursorEvent.h>
    #include <neon/io/MouseButtonEvent.h>
    #include <neon/io/ScrollEvent.h>

namespace neon
{

    CefWebView::NeonCefClient::NeonCefClient(std::shared_ptr<Texture> texture) :
        _renderHandler(new CefTextureRenderHandler(1, 1, std::move(texture))),
        _browser(nullptr)
    {
    }

    CefRefPtr<CefRenderHandler> CefWebView::NeonCefClient::GetRenderHandler()
    {
        return _renderHandler;
    }

    CefRefPtr<CefLifeSpanHandler> CefWebView::NeonCefClient::GetLifeSpanHandler()
    {
        return this;
    }

    CefRefPtr<CefBrowser> CefWebView::NeonCefClient::getBrowser() const
    {
        return _browser;
    }

    void CefWebView::NeonCefClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
    {
        _browser = browser;
    }

    void CefWebView::NeonCefClient::setSize(uint32_t width, uint32_t height)
    {
        _renderHandler->setSize(width, height);
        if (_browser) {
            _browser->GetHost()->WasResized();
        }
    }

    CefWebView::CefWebView() :
        _windowOrigin(0.0f, 0.0f) {};

    void CefWebView::onStart()
    {
        TextureCreateInfo info;
        info.image.width = 2048;
        info.image.height = 2048;
        info.image.depth = 1;
        info.image.format = TextureFormat::B8G8R8A8;
        info.image.mipmaps = 1;

        _texture = std::make_shared<Texture>(getApplication(), "neon::webview", nullptr, info);
        _client = new NeonCefClient(_texture);

        CefWindowInfo window_info;
        window_info.SetAsWindowless(0);

        CefBrowserSettings browser_settings;
        browser_settings.windowless_frame_rate = 60;
        browser_settings.background_color = 0xFFFFFFFF;

        CefBrowserHost::CreateBrowser(window_info, _client, "www.google.com", browser_settings,
                                      nullptr, nullptr);
    }

    void CefWebView::onPreDraw()
    {
        ImGui::ShowDemoWindow();
        ImVec2 max(_texture->getWidth(), _texture->getHeight());

        ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), max);
        if (ImGui::Begin("Webview")) {
            auto size = ImGui::GetContentRegionAvail();

            auto origin = ImGui::GetCursorScreenPos();
            _windowOrigin = rush::Vec2i(origin.x, origin.y).cast<float>();
            _client->setSize(size.x, size.y);

            auto relative = ImVec2(size.x / max.x, size.y / max.y);

            ImGui::Image(_texture, size, ImVec2(0, 0), relative);
        }
        ImGui::End();
    }

    void CefWebView::onCursorMove(const CursorMoveEvent& event)
    {
        auto browser = _client->getBrowser();
        if (browser == nullptr) {
            return;
        }

        _lastMousePosition = event.position;

        CefMouseEvent mouseEvent;
        mouseEvent.x = event.position.x() - _windowOrigin.x();
        mouseEvent.y = event.position.y() - _windowOrigin.y();
        browser->GetHost()->SendMouseMoveEvent(mouseEvent, false);
    }

    void CefWebView::onMouseButton(const MouseButtonEvent& event)
    {
        auto browser = _client->getBrowser();
        if (browser == nullptr) {
            return;
        }

        CefMouseEvent mouseEvent;
        mouseEvent.x = _lastMousePosition.x() - _windowOrigin.x();
        mouseEvent.y = _lastMousePosition.y() - _windowOrigin.y();
        mouseEvent.modifiers = 0;
        CefBrowserHost::MouseButtonType btn = (event.button == MouseButton::BUTTON_PRIMARY     ? MBT_LEFT
                                               : event.button == MouseButton::BUTTON_SECONDARY ? MBT_RIGHT
                                               : event.button == MouseButton::BUTTON_MIDDLE    ? MBT_MIDDLE
                                                                                               : MBT_LEFT);

        bool mouseUp = (event.action == KeyboardAction::RELEASE);
        int clickCount = 1;
        browser->GetHost()->SendMouseClickEvent(mouseEvent, btn, mouseUp, clickCount);
    }

    void CefWebView::onScroll(const ScrollEvent& event)
    {
        auto browser = _client->getBrowser();
        if (browser == nullptr) {
            return;
        }

        CefMouseEvent mouseEvent;
        mouseEvent.x = _lastMousePosition.x() - _windowOrigin.x();
        mouseEvent.y = _lastMousePosition.y() - _windowOrigin.y();
        auto delta = (event.delta * 100.0f).cast<int>();
        browser->GetHost()->SendMouseWheelEvent(mouseEvent, delta.x(), delta.y());
    }
} // namespace neon

#endif