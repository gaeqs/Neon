//
// Created by gaelr on 23/10/2022.
//

#include "Texture.h"

#include <engine/structure/Room.h>

uint64_t TEXTURE_ID_GENERATOR = 1;

Texture::Texture(Room* room, const void* data,
                 const TextureCreateInfo& createInfo) :
        _id(TEXTURE_ID_GENERATOR++),
        _implementation(room->getApplication(), data, createInfo) {
}

#ifdef USE_VULKAN

Texture::Texture(Room* room, VkImageView imageView, VkImageLayout layout,
                 uint32_t width, uint32_t height, uint32_t depth,
                 const SamplerCreateInfo& createInfo) :
        _id(TEXTURE_ID_GENERATOR++),
        _implementation(room->getApplication(), imageView, layout,
                        width, height, depth, createInfo) {
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

uint32_t Texture::getDepth() const {
    return _implementation.getDepth();
}

void Texture::updateData(const char* data,
                         int32_t width, int32_t height, int32_t depth,
                         TextureFormat format) {
    _implementation.updateData(data, width, height, depth, format);
}

