//
// Created by gaeqs on 18/05/25.
//

#ifndef CEFTEXTURERENDERHANDLER_H
#define CEFTEXTURERENDERHANDLER_H

#ifdef USE_CEF

    #include <neon/render/texture/Texture.h>

    #include <cef_render_handler.h>

namespace neon
{
    class CefTextureRenderHandler : public CefRenderHandler
    {
        uint32_t _width, _height;
        std::shared_ptr<Texture> _texture;

    public:
        CefTextureRenderHandler(uint32_t width, uint32_t height, std::shared_ptr<Texture> texture);

        void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;

        void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects,
                     const void* buffer, int width, int height) override;

        bool setSize(uint32_t width, uint32_t height);

        IMPLEMENT_REFCOUNTING(CefTextureRenderHandler);
    };
} // namespace neon

#endif // CEFTEXTURERENDERHANDLER_H

#endif