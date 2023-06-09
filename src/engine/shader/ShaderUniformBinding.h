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

        ShaderUniformBinding(UniformBindingType type_, uint32_t size_)
                : type(type_), size(size_) {}

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
        inline static ShaderUniformBinding buffer(uint32_t size_) {
            return {UniformBindingType::BUFFER, size_};
        }
    };
}


#endif //NEON_SHADERUNIFORMBINDING_H
