//
// Created by gaelr on 21/11/2022.
//

#ifndef NEON_SHADERUNIFORMBINDING_H
#define NEON_SHADERUNIFORMBINDING_H

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

        /**
         * Creates a ShaderUniformBinding representing an image.
         * @return the ShaderUniformBinding.
         */
        inline static ShaderUniformBinding image() {
            return {UniformBindingType::IMAGE, 0};
        }

        /**
         * Creates a ShaderUniformBinding representing a buffer.
         * @param the size of the buffer.
         * @return the ShaderUniformBinding.
         */
        inline static ShaderUniformBinding buffer(uint32_t size) {
            return {UniformBindingType::BUFFER, size};
        }
    };
}


#endif //NEON_SHADERUNIFORMBINDING_H
