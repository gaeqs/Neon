//
// Created by gaelr on 23/10/2022.
//

#include "Texture.h"

#include <engine/structure/Room.h>

uint64_t TEXTURE_ID_GENERATOR = 1;

Texture::Texture(Room* room, const void* data,
                 int32_t width, int32_t height,
                 TextureFormat format) :
        _id(TEXTURE_ID_GENERATOR++),
        _implementation(room->getApplication(), data, width, height, format) {
}

#ifdef USE_VULKAN

Texture::Texture(Room* room, int32_t width, int32_t height,
                 VkImageView imageView,
                 VkImageLayout layout) :
        _id(TEXTURE_ID_GENERATOR++),
        _implementation(room->getApplication(), width, height,
                        imageView, layout) {
}

#endif

uint64_t Texture::getId() const {
    return _id;
}

const Texture::Implementation& Texture::getImplementation() const {
    return _implementation;
}

Texture::Implementation& Texture::getImplementation() {
    return _implementation;
}

uint32_t Texture::getWidth() const {
    return _implementation.getWidth();
}

uint32_t Texture::getHeight() const {
    return _implementation.getHeight();
}

void Texture::updateData(const char* data, int32_t width, int32_t height,
                         TextureFormat format) {
    _implementation.updateData(data, width, height, format);
}

