//
// Created by grial on 21/10/22.
//

#ifndef RVTRACKING_GLTEXTURE_H
#define RVTRACKING_GLTEXTURE_H

#include <cstdint>
#include <utility>
#include <engine/TextureFormat.h>

class Room;

class GLTexture {

    uint32_t _id;
    int32_t _width, _height;

    static std::pair<int32_t, int32_t> toGLFormat(TextureFormat format);

public:

    GLTexture(const GLTexture& other) = delete;

    GLTexture(Room* room, const void* data, int32_t width, int32_t height,
              TextureFormat format);

    ~GLTexture();

    [[nodiscard]] uint32_t getId() const;

    [[nodiscard]] int32_t getWidth() const;

    [[nodiscard]] int32_t getHeight() const;

    void bind(uint32_t index) const;

    void updateData(const void* data, int32_t width, int32_t height,
                    TextureFormat format);

};


#endif //RVTRACKING_GLTEXTURE_H
