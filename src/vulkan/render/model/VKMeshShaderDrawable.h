//
// Created by gaeqs on 02/10/2024.
//

#ifndef VKMESHSHADERDRAWABLE_H
#define VKMESHSHADERDRAWABLE_H

#include "VKDrawable.h"

#include <functional>

namespace neon
{
    class Model;
}

namespace neon::vulkan
{
    class AbstractVKApplication;

    class VKMeshShaderDrawable : public VKDrawable
    {
        AbstractVKApplication* _vkApplication;
        std::function<rush::Vec<3, uint32_t>(const Model&)> _groupsSupplier;

      public:
        VKMeshShaderDrawable(const VKMeshShaderDrawable& other) = delete;

        explicit VKMeshShaderDrawable(Application* application);

        ~VKMeshShaderDrawable() override = default;

        void draw(Material* material, VKCommandBuffer* commandBuffer, const Model& model,
                  ShaderUniformBuffer* globalBuffer) override;

        void setGroupsSupplier(const std::function<rush::Vec<3, uint32_t>(const Model&)>& supplier);
    };
} // namespace neon::vulkan

#endif //VKMESHSHADERDRAWABLE_H
