//
// Created by gaelr on 21/11/2022.
//

#ifndef NEON_SHADERUNIFORMBINDING_H
#define NEON_SHADERUNIFORMBINDING_H

#include <cstdint>
#include <optional>
#include <string>

namespace neon {
    enum class UniformBindingType {
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
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
        static ShaderUniformBinding image() {
            return {UniformBindingType::IMAGE, 0};
        }

        /**
         * Creates a ShaderUniformBinding representing a uniform buffer.
         * @param size_ the size of the buffer.
         * @return the ShaderUniformBinding.
         */
        static ShaderUniformBinding uniformBuffer(uint32_t size_) {
            return {UniformBindingType::UNIFORM_BUFFER, size_};
        }

        /**
         *
         * Creates a ShaderUniformBinding representing a storage buffer.
         * @param size_ the size of the buffer.
         * @return the ShaderUniformBinding.
         */
        static ShaderUniformBinding storageBuffer(uint32_t size_) {
            return {UniformBindingType::STORAGE_BUFFER, size_};
        }
    };

    namespace serialization {
        std::optional<UniformBindingType> toUniformBindingType(std::string s);
    }
}


#endif //NEON_SHADERUNIFORMBINDING_H
