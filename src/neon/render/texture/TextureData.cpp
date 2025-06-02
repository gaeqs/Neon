//
// Created by gaeqs on 6/05/25.
//

#include "TextureData.h"

#include <stb/stb_image.h>

namespace neon
{

    TextureData::TextureData() :
        _data(nullptr),
        _width(0),
        _height(0),
        _depth(0)
    {
    }

    TextureData::TextureData(const std::shared_ptr<std::byte[]>& data, size_t width, size_t height, size_t depth) :
        _data(data),
        _width(width),
        _height(height),
        _depth(depth)
    {
    }

    std::byte* TextureData::getData()
    {
        return _data.get();
    }

    const std::byte* TextureData::getData() const
    {
        return _data.get();
    }

    size_t TextureData::getWidth() const
    {
        return _width;
    }

    size_t TextureData::getHeight() const
    {
        return _height;
    }

    size_t TextureData::getDepth() const
    {
        return _depth;
    }

    TextureData TextureData::fromFile(const cmrc::file& resource)
    {
        const void* data = resource.begin();

        int32_t width, height, channels;
        void* ptr = stbi_load_from_memory(static_cast<const stbi_uc*>(data), static_cast<int32_t>(resource.size()),
                                          &width, &height, &channels, 4);

        auto shared =
            std::shared_ptr<std::byte[]>(static_cast<std::byte*>(ptr), [](std::byte* p) { stbi_image_free(p); });

        return TextureData(std::move(shared), width, height, 1);
    }

    TextureData TextureData::fromFile(const File& resource)
    {
        const void* data = resource.getData();

        int32_t width, height, channels;
        void* ptr = stbi_load_from_memory(static_cast<const stbi_uc*>(data), static_cast<int32_t>(resource.getSize()),
                                          &width, &height, &channels, 4);

        auto shared =
            std::shared_ptr<std::byte[]>(static_cast<std::byte*>(ptr), [](std::byte* p) { stbi_image_free(p); });

        return TextureData(std::move(shared), width, height, 1);
    }

    TextureData TextureData::fromFile(const void* data, size_t size)
    {
        int32_t width, height, channels;
        void* ptr = stbi_load_from_memory(static_cast<const stbi_uc*>(data), static_cast<int32_t>(size), &width,
                                          &height, &channels, 4);

        auto shared =
            std::shared_ptr<std::byte[]>(static_cast<std::byte*>(ptr), [](std::byte* p) { stbi_image_free(p); });

        return TextureData(std::move(shared), width, height, 1);
    }
} // namespace neon