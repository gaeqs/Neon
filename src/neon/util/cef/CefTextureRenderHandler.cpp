//
// Created by gaeqs on 18/05/25.
//

#ifdef USE_CEF

    #include "CefTextureRenderHandler.h"

namespace neon
{

    CefTextureRenderHandler::CefTextureRenderHandler(uint32_t width, uint32_t height,
                                                     std::shared_ptr<Texture> texture) :
        _width(width),
        _height(height),
        _texture(std::move(texture))
    {
    }

    void CefTextureRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
    {
        rect = CefRect(0, 0, _width, _height);
    }

    void CefTextureRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                                          const RectList& dirtyRects, const void* buffer, int width, int height)
    {
        debug() << "Refreshing CEF texture.";
        const char* data = static_cast<const char*>(buffer);

        _texture->updateData(data, std::min(static_cast<uint32_t>(width), _texture->getWidth()),
                             std::min(static_cast<uint32_t>(height), _texture->getHeight()), 1,
                             TextureFormat::B8G8R8A8);
    }

    void CefTextureRenderHandler::setSize(uint32_t width, uint32_t height)
    {
        _width = width;
        _height = height;
    }
} // namespace neon

#endif