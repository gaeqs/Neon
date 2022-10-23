//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_TEXTURE_H
#define RVTRACKING_TEXTURE_H


#include <engine/Identifiable.h>

#ifdef USE_OPENGL

#include <gl/GLTexture.h>

#endif

class Texture : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

#ifdef USE_OPENGL

    using Implementation = GLTexture;
#endif

    uint64_t _id;
    Implementation _implementation;

public:

    Texture(const Texture& other) = delete;

    Texture(const char* data, int32_t width, int32_t height);

    uint64_t getId() const override;

    const Implementation& getImplementation() const;

    Implementation& getImplementation();

    uint32_t getWidth() const;

    uint32_t getHeight() const;

    void updateData(const char* data, int32_t width, int32_t height);

};


#endif //RVTRACKING_TEXTURE_H
