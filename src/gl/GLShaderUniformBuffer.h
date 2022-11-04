//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_GLSHADERUNIFORMBUFFER_H
#define NEON_GLSHADERUNIFORMBUFFER_H

#include <cstdint>

class GLShaderUniformBuffer {

    uint32_t _id;

public:

    GLShaderUniformBuffer();

    ~GLShaderUniformBuffer();

    void setBindingPoint(uint32_t point) const;

    void uploadData(const void* data, size_t size) const;

};


#endif //NEON_GLSHADERUNIFORMBUFFER_H
