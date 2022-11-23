//
// Created by grial on 21/11/22.
//

#ifndef NEON_VKSHADERUNIFORM_H
#define NEON_VKSHADERUNIFORM_H

#include <string>
#include <cstdint>


struct VKShaderUniformBlock {
    std::string name;
    uint32_t index;
    uint32_t binding;
    uint32_t size;
    uint32_t stages;
    uint32_t members;
};

struct VKShaderUniform {
    std::string name;
    uint32_t blockIndex;
    uint32_t offset;
    uint32_t stages;
};


#endif //NEON_VKSHADERUNIFORM_H
