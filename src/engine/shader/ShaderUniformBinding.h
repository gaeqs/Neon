//
// Created by gaelr on 21/11/2022.
//

#ifndef NEON_SHADERUNIFORMBINDING_H
#define NEON_SHADERUNIFORMBINDING_H

#include <engine/structure/IdentifiableWrapper.h>
#include <engine/render/Texture.h>
#include <cstdint>

namespace neon {
    enum class UniformBindingType {
        BUFFER,
        IMAGE
    };

    struct ShaderUniformBinding {
        UniformBindingType type;
        uint32_t size;

        ShaderUniformBinding(UniformBindingType type, uint32_t size)
                : type(type), size(size) {}
    };
}


#endif //NEON_SHADERUNIFORMBINDING_H
