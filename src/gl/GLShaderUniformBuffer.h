//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_GLSHADERUNIFORMBUFFER_H
#define NEON_GLSHADERUNIFORMBUFFER_H

#include <cstdint>
#include <cstdlib>

class Application;

class GLShaderUniformBuffer {

    uint32_t _id;
    size_t _size;

public:

    GLShaderUniformBuffer(Application* application, uint32_t size);

    ~GLShaderUniformBuffer();

    void setBindingPoint(uint32_t point) const;

    void uploadData(const void* data, size_t size) const;

    void prepareForFrame() const;

};


#endif //NEON_GLSHADERUNIFORMBUFFER_H
