//
// Created by gaelr on 21/11/2022.
//

#ifndef NEON_VKSHADERUNIFORMDESCRIPTOR_H
#define NEON_VKSHADERUNIFORMDESCRIPTOR_H

#include <vulkan/VKApplication.h>
#include <engine/Application.h>
#include <engine/structure/Identifiable.h>
#include <engine/shader/ShaderUniformBinding.h>

namespace neon::vulkan {
    class VKShaderUniformDescriptor {

        static VkDescriptorType getType(UniformBindingType type);

        AbstractVKApplication* _vkApplication;
        std::vector<ShaderUniformBinding> _bindings;
        VkDescriptorSetLayout _descriptorSetLayout;


    public:

        VKShaderUniformDescriptor(
                const VKShaderUniformDescriptor& other) = delete;

        VKShaderUniformDescriptor(
                Application* application,
                const std::vector<ShaderUniformBinding>& bindings);

        ~VKShaderUniformDescriptor();

        [[nodiscard]] AbstractVKApplication* getVkApplication() const;

        [[nodiscard]] const std::vector<ShaderUniformBinding>&
        getBindings() const;

        [[nodiscard]] VkDescriptorSetLayout getDescriptorSetLayout() const;

    };
}

#endif //NEON_VKSHADERUNIFORMDESCRIPTOR_H
