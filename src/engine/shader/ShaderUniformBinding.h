//
// Created by gaelr on 21/11/2022.
//

#ifndef NEON_SHADERUNIFORMBINDING_H
#define NEON_SHADERUNIFORMBINDING_H

#include <engine/IdentifiableWrapper.h>
#include <engine/Texture.h>
#include <cstdint>

enum class UniformBindingType {
    BUFFER,
    IMAGE
};

struct ShaderUniformBinding {
    UniformBindingType type;
    uint32_t size;
    IdentifiableWrapper<Texture> texture;
};


#endif //NEON_SHADERUNIFORMBINDING_H