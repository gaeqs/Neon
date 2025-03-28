//
// Created by gaelr on 21/11/2022.
//

#include "VKShaderUniformDescriptor.h"

#include <stdexcept>

namespace neon::vulkan
{
    VkDescriptorType VKShaderUniformDescriptor::getType(UniformBindingType type)
    {
        switch (type) {
            case UniformBindingType::IMAGE:
                return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case UniformBindingType::STORAGE_BUFFER:
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case UniformBindingType::UNIFORM_BUFFER:
            default:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }

    VKShaderUniformDescriptor::VKShaderUniformDescriptor(Application* application,
                                                         const std::vector<ShaderUniformBinding>& bindings) :
        _vkApplication(dynamic_cast<AbstractVKApplication*>(application->getImplementation())),
        _bindings(bindings),
        _descriptorSetLayout(VK_NULL_HANDLE)
    {
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.reserve(bindings.size());

        for (const auto& binding : bindings) {
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = static_cast<uint32_t>(layoutBindings.size());
            uboLayoutBinding.descriptorType = getType(binding.type);
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            layoutBindings.push_back(uboLayoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
        layoutInfo.pBindings = layoutBindings.data();

        if (vkCreateDescriptorSetLayout(_vkApplication->getDevice()->getRaw(), &layoutInfo, nullptr,
                                        &_descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }
    }

    VKShaderUniformDescriptor::~VKShaderUniformDescriptor()
    {
        vkDestroyDescriptorSetLayout(_vkApplication->getDevice()->getRaw(), _descriptorSetLayout, nullptr);
    }

    AbstractVKApplication* VKShaderUniformDescriptor::getVkApplication() const
    {
        return _vkApplication;
    }

    VkDescriptorSetLayout VKShaderUniformDescriptor::getDescriptorSetLayout() const
    {
        return _descriptorSetLayout;
    }

    const std::vector<ShaderUniformBinding>& VKShaderUniformDescriptor::getBindings() const
    {
        return _bindings;
    }
} // namespace neon::vulkan
