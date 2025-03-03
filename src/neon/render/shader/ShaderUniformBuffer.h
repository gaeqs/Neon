//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_SHADERUNIFORMBUFFER_H
#define NEON_SHADERUNIFORMBUFFER_H

#include <neon/structure/Asset.h>

#ifdef USE_VULKAN

#include <vulkan/render/shader/VKShaderUniformBuffer.h>

#endif

namespace neon {
    class Application;

    class ShaderUniformDescriptor;

    class CommandBuffer;

    class ShaderUniformBuffer : public Asset {
    public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKShaderUniformBuffer;
#endif

    private:
        std::shared_ptr<ShaderUniformDescriptor> _descriptor;
        Implementation _implementation;

    public:
        ShaderUniformBuffer(const ShaderUniformBuffer& other) = delete;

        ShaderUniformBuffer(std::string name, const std::shared_ptr<ShaderUniformDescriptor>& descriptor);

        [[nodiscard]] const Implementation& getImplementation() const;

        [[nodiscard]] Implementation& getImplementation();

        [[nodiscard]] const std::shared_ptr<ShaderUniformDescriptor>&
        getDescriptor() const;

        void uploadData(uint32_t index, const void* data, size_t size, size_t offset = 0);

        void clearData(uint32_t index);

        void* fetchData(uint32_t index);

        const void* fetchData(uint32_t index) const;

        void setTexture(uint32_t index, std::shared_ptr<Texture> texture);

        template<class T>
        void uploadData(uint32_t index, const T& data) {
            uploadData(index, &data, sizeof(T));
        }

        void prepareForFrame(const CommandBuffer* commandBuffer);

        /**
         * Transfers data from the GPU to the CPU for the uniform or storage buffer assigned to the given index.
         *
         * This function immediately updates the CPU-side copy of the buffer's contents, making the
         * latest data available for retrieval. You can then access the updated information by calling
         * the fetchData function.
         *
         * This will only work if the buffer type of the binding is UniformBindingBufferType::SIMPLE.
         */
        void transferDataFromGPU(uint32_t index);
    };
}


#endif //NEON_SHADERUNIFORMBUFFER_H
