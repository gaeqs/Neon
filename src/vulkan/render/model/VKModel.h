//
// Created by gaelr on 15/11/2022.
//

#ifndef NEON_VKMODEL_H
#define NEON_VKMODEL_H

#include <neon/render/model/InstanceData.h>
#include <vulkan/render/model/VKMesh.h>

namespace neon
{
    class Model;

    class Application;

    class Material;

    class CommandBuffer;

    class ShaderUniformBuffer;
} // namespace neon

namespace neon::vulkan
{
    class AbstractVKApplication;

    class VKModel
    {
        Application* _application;
        Model* _model;

      public:
        VKModel(const VKModel& other) = delete;

        VKModel(Application* application, Model* model);

        void draw(const Material* material) const;

        void drawOutside(const Material* material, const CommandBuffer* commandBuffer) const;
    };
} // namespace neon::vulkan

#endif //NEON_VKMODEL_H
