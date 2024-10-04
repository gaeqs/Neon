//
// Created by gaeqs on 02/10/2024.
//

#ifndef VKMESHSHADERDRAWABLE_H
#define VKMESHSHADERDRAWABLE_H

#include "VKDrawable.h"

#include <functional>

namespace neon {
    class Model;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKMeshShaderDrawable : public VKDrawable {
        AbstractVKApplication* _vkApplication;
        std::function<rush::Vec<3, uint32_t>(const Model&)> _groupsSupplier;

    public:
        VKMeshShaderDrawable(const VKMeshShaderDrawable& other) = delete;

        explicit VKMeshShaderDrawable(Application* application);

        ~VKMeshShaderDrawable() override = default;

        void draw(const Material* material,
                  VkCommandBuffer commandBuffer,
                  const Model& model,
                  const ShaderUniformBuffer* globalBuffer) override;


        void setGroupsSupplier(
            const std::function<rush::Vec<3, uint32_t>(const Model&)>&
            supplier);
    };
}


#endif //VKMESHSHADERDRAWABLE_H
