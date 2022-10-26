//
// Created by grial on 21/10/22.
//

#include "GLTexture.h"

#include <glad/glad.h>
#include <stdexcept>


int32_t GLTexture::toGLFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::RGB:
            return GL_RGB;
        case TextureFormat::BGR:
            return GL_BGR;
        case TextureFormat::RGBA:
            return GL_RGBA;
        case TextureFormat::ARGB:
            throw std::runtime_error(
                    "OpenGL doesn't support ARGB texture format!");
        case TextureFormat::BGRA:
            return GL_RGBA;
        case TextureFormat::ABGR:
            throw std::runtime_error(
                    "OpenGL doesn't support ABGR texture format!");
    }
}

GLTexture::GLTexture(const char* data, int32_t width, int32_t height,
                     TextureFormat format) :
        _width(width),
        _height(height) {
    glActiveTexture(0);
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    updateData(data, _width, _height, format);
}

GLTexture::~GLTexture() {
    glDeleteTextures(1, &_id);
}

uint32_t GLTexture::getId() const {
    return _id;
}

int32_t GLTexture::getWidth() const {
    return _width;
}

int32_t GLTexture::getHeight() const {
    return _height;
}

void GLTexture::bind(uint32_t index) const {
    glActiveTexture(index);
    glBindTexture(GL_TEXTURE_2D, _id);
}

void GLTexture::updateData(const char* data, int32_t width, int32_t height,
                           TextureFormat format) {
    _width = width;
    _height = height;
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 toGLFormat(format), GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
