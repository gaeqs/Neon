//
// Created by gaelr on 21/11/2022.
//

#include "ShaderUniformBinding.h"

#include <algorithm>

namespace neon {
    std::optional<UniformBindingType> serialization::toUniformBindingType(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });
        if (s == "IMAGE") return UniformBindingType::IMAGE;
        if (s == "STORAGE_BUFFER") return UniformBindingType::STORAGE_BUFFER;
        if (s == "UNIFORM_BUFFER") return UniformBindingType::UNIFORM_BUFFER;
        return {};
    }
}
