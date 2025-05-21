//
// Created by gaeqs on 18/05/25.
//

#ifdef USE_CEF

    #include "CefWebView.h"

    #include <imgui.h>
    #include <imgui_demo.cpp>
    #include <imgui_internal.h>
    #include <neon/io/CharEvent.h>
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
        if (_renderHandler->setSize(width, height) && _browser) {
            _browser->GetHost()->WasResized();
        }
    }

    void CefWebView::refreshModifiers(const MouseButtonEvent& event)
    {
        _modifiers.capsLock = event.isModifierActive(KeyboardModifier::CAPS_LOCK);
        _modifiers.shift = event.isModifierActive(KeyboardModifier::SHIFT);
        _modifiers.control = event.isModifierActive(KeyboardModifier::CONTROL);
        _modifiers.alt = event.isModifierActive(KeyboardModifier::ALT);

        bool active = event.action == KeyboardAction::PRESS;
        switch (event.button) {
            case MouseButton::BUTTON_PRIMARY:
                _modifiers.primary = active;
                break;
            case MouseButton::BUTTON_SECONDARY:
                _modifiers.secondary = active;
                break;
            case MouseButton::BUTTON_MIDDLE:
                _modifiers.middle = active;
                break;
            default:
                break;
        }
    }

    int CefWebView::fetchModifiers() const
    {
        int modifiers = 0;
        if (_modifiers.capsLock) {
            modifiers += EVENTFLAG_CAPS_LOCK_ON;
        }
        if (_modifiers.shift) {
            modifiers += EVENTFLAG_SHIFT_DOWN;
        }
        if (_modifiers.control) {
            modifiers += EVENTFLAG_CONTROL_DOWN;
        }
        if (_modifiers.alt) {
            modifiers += EVENTFLAG_ALT_DOWN;
        }
        if (_modifiers.primary) {
            modifiers += EVENTFLAG_LEFT_MOUSE_BUTTON;
        }
        if (_modifiers.secondary) {
            modifiers += EVENTFLAG_RIGHT_MOUSE_BUTTON;
        }
        if (_modifiers.middle) {
            modifiers += EVENTFLAG_MIDDLE_MOUSE_BUTTON;
        }

        return modifiers;
    }

    CefWebView::CefWebView(const std::string& url) :
        _startURL(url),
        _windowOrigin(0.0f, 0.0f),
        _modifiers()
    {
    }

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

        CefBrowserHost::CreateBrowser(window_info, _client, _startURL, browser_settings, nullptr, nullptr);
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

            if (auto browser = _client->getBrowser()) {
                browser->GetHost()->SetFocus(ImGui::IsWindowFocused());
            }
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
        mouseEvent.modifiers = fetchModifiers();

        browser->GetHost()->SendMouseMoveEvent(mouseEvent, false);
    }

    void CefWebView::onMouseButton(const MouseButtonEvent& event)
    {
        auto browser = _client->getBrowser();
        if (browser == nullptr) {
            return;
        }

        refreshModifiers(event);

        CefMouseEvent mouseEvent;
        mouseEvent.x = _lastMousePosition.x() - _windowOrigin.x();
        mouseEvent.y = _lastMousePosition.y() - _windowOrigin.y();
        mouseEvent.modifiers = fetchModifiers();
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
        mouseEvent.modifiers = fetchModifiers();
        auto delta = (event.delta * 100.0f).cast<int>();
        browser->GetHost()->SendMouseWheelEvent(mouseEvent, delta.x(), delta.y());
    }

    void CefWebView::onKey(const KeyboardEvent& event)
    {
        auto browser = _client->getBrowser();
        if (browser == nullptr) {
            return;
        }

        CefKeyEvent keyEvent;
        keyEvent.native_key_code = static_cast<int>(event.key);
        keyEvent.windows_key_code = keyEvent.native_key_code;
        keyEvent.modifiers = fetchModifiers();
        keyEvent.is_system_key = false;
        keyEvent.type = event.action == KeyboardAction::RELEASE ? KEYEVENT_RAWKEYDOWN
                        : event.action == KeyboardAction::PRESS ? KEYEVENT_KEYUP
                                                                : KEYEVENT_RAWKEYDOWN;
        browser->GetHost()->SendKeyEvent(keyEvent);
    }

    void CefWebView::onChar(const CharEvent& event)
    {
        auto browser = _client->getBrowser();
        if (browser == nullptr) {
            return;
        }

        CefKeyEvent keyEvent;
        keyEvent.type = KEYEVENT_CHAR;
        keyEvent.character = event.character;
        keyEvent.unmodified_character = event.character;
        keyEvent.windows_key_code = static_cast<int>(event.character);
        keyEvent.modifiers = fetchModifiers();
        keyEvent.is_system_key = false;
        browser->GetHost()->SendKeyEvent(keyEvent);
    }
} // namespace neon

#endif