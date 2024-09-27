//
// Created by gaelr on 15/11/2022.
//

#ifndef NEON_VKMODEL_H
#define NEON_VKMODEL_H

#include <memory>
#include <neon/render/model/InstanceData.h>

#include <neon/render/model/ModelCreateInfo.h>
#include <vulkan/render/model/VKMesh.h>
#include <vulkan/render/buffer/StagingBuffer.h>

namespace neon {
    class Application;

    class Material;

    class CommandBuffer;

    class ShaderUniformBuffer;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKModel {
        Application* _application;
        std::vector<VKMesh*> _meshes;
        InstanceData* _instanceData;
        Pipeline _pipeline;

        static std::vector<Mesh::Implementation*> getMeshImplementations(
            const std::vector<std::shared_ptr<Mesh>>& meshes);

    public:
        VKModel(const VKModel& other) = delete;

        VKModel(Application* application,
                const ModelCreateInfo& info,
                InstanceData* instanceData);

        void draw(const Material* material,
                  const ShaderUniformBuffer* modelBuffer) const;

        void drawOutside(const Material* material,
                         const ShaderUniformBuffer* modelBuffer,
                         const CommandBuffer* commandBuffer) const;
    };
}

#endif //NEON_VKMODEL_H
