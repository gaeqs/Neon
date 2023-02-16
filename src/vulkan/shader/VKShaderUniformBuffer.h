//
// Created by grial on 14/11/22.
//

#ifndef NEON_VKSHADERUNIFORMBUFFER_H
#define NEON_VKSHADERUNIFORMBUFFER_H

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include <engine/structure/IdentifiableWrapper.h>
#include <engine/shader/ShaderUniformBinding.h>
#include <vulkan/shader/VKShaderUniformDescriptor.h>

namespace neon {
    class Application;

    class Buffer;

    class Texture;

    class ShaderUniformDescriptor;
}

namespace neon::vulkan {
    class VKApplication;

    class VKShaderUniformBuffer {

        VKApplication* _vkApplication;
        VkDescriptorPool _descriptorPool;

        std::vector<std::shared_ptr<Buffer>> _buffers;
        std::vector<VkDescriptorSet> _descriptorSets;
        std::vector<UniformBindingType> _types;
        std::vector<std::vector<uint32_t>> _updated;
        std::vector<std::vector<char>> _data;
        std::vector<IdentifiableWrapper<Texture>> _textures;

        uint32_t _bindingPoint; // In vulkan this is the "set" parameter.

    public:

        explicit VKShaderUniformBuffer(
                const std::shared_ptr<ShaderUniformDescriptor>& descriptor);

        ~VKShaderUniformBuffer();

        void setBindingPoint(uint32_t point);

        void uploadData(uint32_t index, const void* data, size_t size);

        void setTexture(uint32_t index, IdentifiableWrapper<Texture> texture);

        void prepareForFrame();

        void bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout) const;
    };
}

#endif //NEON_VKSHADERUNIFORMBUFFER_H
