//
// Created by grial on 21/10/22.
//

#include "TextureCollection.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

TextureCollection::TextureCollection(Room* room)
        : IdentifiableCollection(room) {
}

IdentifiableWrapper<Texture>
TextureCollection::createTextureFromPNG(const cmrc::file& resource, bool srgb) {
    return createTextureFromPNG(resource.begin(), resource.size(), srgb);
}

IdentifiableWrapper<Texture>
TextureCollection::createTextureFromPNG(const std::string& path, bool srgb) {
    int32_t width, height, channels;
    auto info = stbi_info(path.c_str(), &width, &height, &channels);
    auto ptr = stbi_load(path.c_str(), &width, &height, &channels, 4);
    auto image = create((const char*) ptr, width, height,
                        srgb ? TextureFormat::R8G8B8A8_SRGB
                             : TextureFormat::R8G8B8A8);
    stbi_image_free(ptr);
    return image;
}

IdentifiableWrapper<Texture>
TextureCollection::createTextureFromPNG(const void* data,
                                        uint32_t size, bool srgb) {
    int32_t width, height, channels;
    auto ptr = stbi_load_from_memory(
            static_cast<const uint8_t*>(data),
            static_cast<int32_t>(size),
            &width, &height, &channels, 4);
    auto image = create((const char*) ptr, width, height,
                        srgb ? TextureFormat::R8G8B8A8_SRGB
                             : TextureFormat::R8G8B8A8);
    stbi_image_free(ptr);
    return image;
}
