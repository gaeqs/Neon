//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_SHADERUNIFORMBUFFER_H
#define NEON_SHADERUNIFORMBUFFER_H

#include <engine/Identifiable.h>

#ifdef USE_OPENGL

#include <gl/GLShaderUniformBuffer.h>

#endif

class ShaderUniformBuffer : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

public:
#ifdef USE_OPENGL
    using Implementation = GLShaderUniformBuffer;
#endif

private:

    uint64_t _id;
    Implementation _implementation;

public:

    ShaderUniformBuffer();

    [[nodiscard]] uint64_t getId() const override;

    void setBindingPoint(uint32_t point) const;

    void uploadData(const void* data, size_t size) const;

    template<class T>
    void uploadData(const T& data) const {
        uploadData(&data, sizeof(T));
    }

};


#endif //NEON_SHADERUNIFORMBUFFER_H
