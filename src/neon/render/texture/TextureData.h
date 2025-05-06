//
// Created by gaeqs on 6/05/25.
//

#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H
#include <memory>

#include <cmrc/cmrc.hpp>
#include <neon/filesystem/File.h>

namespace neon
{

    class TextureData
    {
        std::shared_ptr<std::byte[]> _data;

        size_t _width;
        size_t _height;

        size_t _depth;

      public:
        TextureData();

        TextureData(const std::shared_ptr<std::byte[]>& data, size_t width, size_t height, size_t depth);

        [[nodiscard]] std::byte* getData();

        [[nodiscard]] const std::byte* getData() const;

        [[nodiscard]] size_t getWidth() const;

        [[nodiscard]] size_t getHeight() const;

        [[nodiscard]] size_t getDepth() const;

        [[nodiscard]] static TextureData fromFile(const cmrc::file& resource);

        [[nodiscard]] static TextureData fromFile(const File& resource);

        [[nodiscard]] static TextureData fromFile(const void* data, size_t size);
    };
} // namespace neon

#endif // TEXTUREDATA_H
