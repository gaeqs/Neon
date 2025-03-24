//
// Created by gaelr on 21/11/2022.
//

#ifndef NEON_SHADERUNIFORMBINDING_H
#define NEON_SHADERUNIFORMBINDING_H

#include <cstdint>
#include <optional>
#include <string>

namespace neon
{
    enum class UniformBindingType
    {
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
        IMAGE
    };

    enum class UniformBindingBufferType
    {
        /**
         * Use a staging buffer for this binding.
         *
         * Staging buffers act as intermediaries between CPU updates and GPU usage. They prevent
         * conflicts that may arise when new data is written before the GPU finishes processing
         * the previous frame’s data. This buffer type is ideal for scenarios where the uniform data
         * is updated frequently.
         */
        STAGING,

        /**
         * Use a simple (non-staging) buffer for this binding.
         *
         * Simple buffers directly map updates to the GPU buffer without an intermediate staging step.
         * This option is best suited for cases where data updates are infrequent, reducing overhead
         * and memory allocation when synchronization concerns are minimal.
         */
        SIMPLE
    };

    struct ShaderUniformBinding
    {
        UniformBindingType type;
        UniformBindingBufferType bufferType;
        uint32_t size;

        ShaderUniformBinding(UniformBindingType type_, UniformBindingBufferType bufferType_, uint32_t size_) :
            type(type_),
            bufferType(bufferType_),
            size(size_)
        {
        }

        /**
         * Creates a ShaderUniformBinding representing an image.
         * @return the ShaderUniformBinding.
         */
        static ShaderUniformBinding image()
        {
            return {UniformBindingType::IMAGE, UniformBindingBufferType::STAGING, 0};
        }

        /**
         * Creates a ShaderUniformBinding representing a uniform buffer.
         * @param size_ the size of the buffer.
         * @param bufferType_ the buffer type used by this binding.
         * @return the ShaderUniformBinding.
         */
        static ShaderUniformBinding uniformBuffer(
            uint32_t size_, UniformBindingBufferType bufferType_ = UniformBindingBufferType::STAGING)
        {
            return {UniformBindingType::UNIFORM_BUFFER, bufferType_, size_};
        }

        /**
         *
         * Creates a ShaderUniformBinding representing a storage buffer.
         * @param size_ the size of the buffer.
         *          * @param bufferType_ the buffer type used by this binding.

         * @return the ShaderUniformBinding.
         */
        static ShaderUniformBinding storageBuffer(
            uint32_t size_, UniformBindingBufferType bufferType_ = UniformBindingBufferType::STAGING)
        {
            return {UniformBindingType::STORAGE_BUFFER, bufferType_, size_};
        }
    };

    namespace serialization
    {
        std::optional<UniformBindingType> toUniformBindingType(std::string s);
    }
} // namespace neon

#endif //NEON_SHADERUNIFORMBINDING_H
