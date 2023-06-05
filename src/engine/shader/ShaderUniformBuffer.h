//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_SHADERUNIFORMBUFFER_H
#define NEON_SHADERUNIFORMBUFFER_H

#include <engine/structure/Asset.h>

#ifdef USE_VULKAN

#include <vulkan/shader/VKShaderUniformBuffer.h>

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

        ShaderUniformBuffer(
                std::string name,
                const std::shared_ptr<ShaderUniformDescriptor>& descriptor);

        [[nodiscard]] const Implementation& getImplementation() const;

        [[nodiscard]] Implementation& getImplementation();

        [[nodiscard]] const std::shared_ptr<ShaderUniformDescriptor>&
        getDescriptor() const;

        void setBindingPoint(uint32_t point);

        void uploadData(uint32_t index, const void* data, size_t size);

        void setTexture(uint32_t index, std::shared_ptr<Texture> texture);

        template<class T>
        void uploadData(uint32_t index, const T& data) {
            uploadData(index, &data, sizeof(T));
        }

        void prepareForFrame(const CommandBuffer* commandBuffer);

    };
}


#endif //NEON_SHADERUNIFORMBUFFER_H
