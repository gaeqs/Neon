//
// Created by grial on 21/10/22.
//

#include "TextureCollection.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

IdentifiableWrapper<Texture>
TextureCollection::createTextureFromPNG(const cmrc::file& resource) {
    return createTextureFromPNG(resource.begin(), resource.size());
}

IdentifiableWrapper<Texture>
TextureCollection::createTextureFromPNG(const void* data, uint32_t size) {
    int32_t width, height, channels;
    auto ptr = stbi_load_from_memory(
            static_cast<const uint8_t*>(data),
            static_cast<int32_t>(size),
            &width, &height, &channels, 4);
    auto image = create((const char*) ptr, width, height, TextureFormat::RGBA);
    stbi_image_free(ptr);
    return image;
}