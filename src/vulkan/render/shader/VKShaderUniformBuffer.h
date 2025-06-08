//
// Created by grial on 14/11/22.
//

#ifndef NEON_VKSHADERUNIFORMBUFFER_H
#define NEON_VKSHADERUNIFORMBUFFER_H

#include <vector>
#include <memory>
#include <neon/render/shader/ShaderUniformBinding.h>
#include <neon/render/texture/SampledTexture.h>
#include <vulkan/VKResource.h>
#include <vulkan/render/shader/VKShaderUniformDescriptor.h>

namespace neon
{
    class Application;

    class Texture;

    class ShaderUniformDescriptor;

    class CommandBuffer;
} // namespace neon

namespace neon::vulkan
{
    class AbstractVKApplication;

    class Buffer;

    class VKShaderUniformBuffer : public VKResource
    {
        AbstractVKApplication* _vkApplication;
        VkDescriptorPool _descriptorPool;

        std::vector<std::shared_ptr<Buffer>> _buffers;
        std::vector<VkDescriptorSet> _descriptorSets;
        std::vector<ShaderUniformBinding> _bindings;
        std::vector<std::vector<uint32_t>> _updated;
        std::vector<std::vector<char>> _data;
        std::vector<std::shared_ptr<SampledTexture>> _textures;

        uint32_t _bindingPoint; // In vulkan this is the "set" parameter.

      public:
        explicit VKShaderUniformBuffer(const std::shared_ptr<ShaderUniformDescriptor>& descriptor);

        ~VKShaderUniformBuffer();

        void uploadData(uint32_t index, const void* data, size_t size, size_t offset);

        void clearData(uint32_t index);

        void* fetchData(uint32_t index);

        const void* fetchData(uint32_t index) const;

        void setTexture(uint32_t index, std::shared_ptr<SampledTexture> texture);

        void prepareForFrame(const CommandBuffer* commandBuffer);

        void transferDataFromGPU(uint32_t index);

        void bind(VKCommandBuffer* commandBuffer, VkPipelineLayout layout, uint32_t bindingPoint);
    };
} // namespace neon::vulkan

#endif // NEON_VKSHADERUNIFORMBUFFER_H
