//
// Created by grial on 21/11/22.
//

#ifndef NEON_VKSHADERUNIFORM_H
#define NEON_VKSHADERUNIFORM_H

#include <string>
#include <cstdint>

struct VKShaderUniform {
    std::string name;
    uint32_t offset;
    uint32_t stages;
};


#endif //NEON_VKSHADERUNIFORM_H
