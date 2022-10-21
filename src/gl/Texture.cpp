//
// Created by grial on 21/10/22.
//

#include "Texture.h"

#include <glad/glad.h>


Texture::Texture(const char* data, int32_t width, int32_t height) :
        _width(width),
        _height(height) {
    glActiveTexture(0);
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture() {
    glDeleteTextures(1, &_id);
}

uint32_t Texture::getId() const {
    return _id;
}

int32_t Texture::getWidth() const {
    return _width;
}

int32_t Texture::getHeight() const {
    return _height;
}

void Texture::bind(uint32_t index) const {
    glActiveTexture(index);
    glBindTexture(GL_TEXTURE_2D, _id);
}
