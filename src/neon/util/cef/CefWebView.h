//
// Created by gaeqs on 18/05/25.
//

#ifndef CEFWEBVIEW_H
#define CEFWEBVIEW_H

#ifdef USE_CEF

    #include "CefTextureRenderHandler.h"
    #include <cef_client.h>
    #include <neon/structure/Component.h>

namespace neon
{
    class CefWebView : public Component
    {
        class NeonCefClient : public CefClient, public CefLifeSpanHandler
        {
            CefRefPtr<CefTextureRenderHandler> _renderHandler;
            CefRefPtr<CefBrowser> _browser;

          public:
            explicit NeonCefClient(std::shared_ptr<Texture> texture);


            CefRefPtr<CefRenderHandler> GetRenderHandler() override;

            CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;

            CefRefPtr<CefBrowser> getBrowser() const;

            void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

            void setSize(uint32_t width, uint32_t height);

            IMPLEMENT_REFCOUNTING(NeonCefClient);
        };

        std::shared_ptr<Texture> _texture;
        CefRefPtr<NeonCefClient> _client;
        ImVec2 _windowOrigin;
        rush::Vec2f _lastMousePosition;

      public:
        CefWebView();

        void onStart() override;

        void onPreDraw() override;

        void onCursorMove(const CursorMoveEvent& event) override;

        void onMouseButton(const MouseButtonEvent& event) override;

        void onScroll(const ScrollEvent& event) override;
    };
} // namespace neon

#endif

#endif // CEFWEBVIEW_H