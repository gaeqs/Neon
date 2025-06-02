//
// Created by gaeqs on 29/09/2024.
//

#ifndef SHADERUNIFORM_H
#define SHADERUNIFORM_H

#include <vector>
#include <neon/render/texture/TextureCreateInfo.h>

namespace neon
{
    struct ShaderUniform
    {
        std::string name = "";
        std::optional<uint32_t> set = {};
        std::optional<uint32_t> binding = {};
        uint32_t stages = 0;
    };

    struct ShaderUniformBlock : ShaderUniform
    {
        uint32_t sizeInBytes = 0;
        std::optional<uint32_t> offset = 0;
        std::vector<std::string> memberNames = {};
    };

    struct ShaderUniformSampler : ShaderUniform
    {
        TextureViewType type = TextureViewType::NORMAL_2D;
    };
} // namespace neon

#endif //SHADERUNIFORM_H
