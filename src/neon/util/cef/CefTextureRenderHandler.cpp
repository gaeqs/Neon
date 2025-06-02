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
        const std::byte* data = static_cast<const std::byte*>(buffer);

        if (auto modifiable = _texture->asModifiable()) {
            rush::Vec3ui size = rush::min(_texture->getDimensions(), rush::Vec3ui(width, height, 1));

            modifiable.value()->updateData(data, rush::Vec3ui(0), size, 0, 1, nullptr);
        }
    }

    bool CefTextureRenderHandler::setSize(uint32_t width, uint32_t height)
    {
        if (_width != width || _height != height) {
            _width = width;
            _height = height;
            return true;
        }
        return false;
    }
} // namespace neon

#endif