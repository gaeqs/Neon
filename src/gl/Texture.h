//
// Created by grial on 21/10/22.
//

#ifndef RVTRACKING_TEXTURE_H
#define RVTRACKING_TEXTURE_H

#include <cstdint>

class Texture {

    uint32_t _id;
    int32_t _width, _height;

public:

    Texture(const Texture& other) = delete;

    Texture(const char* data, int32_t width, int32_t height);

    ~Texture();

    uint32_t getId() const;

    int32_t getWidth() const;

    int32_t getHeight() const;

    void bind(uint32_t index) const;

    void updateData(const char* data, int32_t width, int32_t height);

};


#endif //RVTRACKING_TEXTURE_H
