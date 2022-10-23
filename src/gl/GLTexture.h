//
// Created by grial on 21/10/22.
//

#ifndef RVTRACKING_GLTEXTURE_H
#define RVTRACKING_GLTEXTURE_H

#include <cstdint>

class GLTexture {

    uint32_t _id;
    int32_t _width, _height;

public:

    GLTexture(const GLTexture& other) = delete;

    GLTexture(const char* data, int32_t width, int32_t height);

    ~GLTexture();

    uint32_t getId() const;

    int32_t getWidth() const;

    int32_t getHeight() const;

    void bind(uint32_t index) const;

    void updateData(const char* data, int32_t width, int32_t height);

};


#endif //RVTRACKING_GLTEXTURE_H
