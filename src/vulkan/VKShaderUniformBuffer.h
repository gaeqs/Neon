//
// Created by grial on 14/11/22.
//

#ifndef NEON_VKSHADERUNIFORMBUFFER_H
#define NEON_VKSHADERUNIFORMBUFFER_H

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

class VKApplication;

class Application;

class Buffer;

class VKShaderUniformBuffer {

    VKApplication* _vkApplication;
    VkDescriptorSetLayout _descriptorSetLayout;
    VkDescriptorPool _descriptorPool;

    std::vector<std::shared_ptr<Buffer>> _buffers;
    std::vector<VkDescriptorSet> _descriptorSets;
    std::vector<bool> _updated;
    uint32_t _currentImage;

    size_t _maxSize;
    std::vector<char> _data;

    uint32_t _bindingPoint; // In vulkan this is the "set" parameter.

public:

    VKShaderUniformBuffer(Application* application, uint32_t size);

    ~VKShaderUniformBuffer();

    void setBindingPoint(uint32_t point);

    void uploadData(const void* data, size_t size);

    void prepareForFrame();

    VkDescriptorSetLayout getDescriptorSetLayout() const;

    void bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout) const;
};


#endif //NEON_VKSHADERUNIFORMBUFFER_H
