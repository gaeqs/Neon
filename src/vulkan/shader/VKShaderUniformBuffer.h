//
// Created by grial on 14/11/22.
//

#ifndef NEON_VKSHADERUNIFORMBUFFER_H
#define NEON_VKSHADERUNIFORMBUFFER_H

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include <engine/shader/ShaderUniformBinding.h>
#include <vulkan/shader/VKShaderUniformDescriptor.h>

class VKApplication;

class Application;

class Buffer;

class VKShaderUniformBuffer {

    VKApplication* _vkApplication;
    VkDescriptorPool _descriptorPool;

    std::vector<std::vector<std::shared_ptr<Buffer>>> _buffers;
    std::vector<VkDescriptorSet> _descriptorSets;
    std::vector<bool> _updated;
    std::vector<std::vector<char>> _data;

    uint32_t _currentImage;
    uint32_t _bindingPoint; // In vulkan this is the "set" parameter.

public:

    VKShaderUniformBuffer(const VKShaderUniformDescriptor& descriptor);

    ~VKShaderUniformBuffer();

    void setBindingPoint(uint32_t point);

    void uploadData(uint32_t index, const void* data, size_t size);

    void prepareForFrame();

    VkDescriptorSetLayout getDescriptorSetLayout() const;

    void bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout) const;
};


#endif //NEON_VKSHADERUNIFORMBUFFER_H
