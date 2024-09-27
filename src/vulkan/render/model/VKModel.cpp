//
// Created by gaelr on 15/11/2022.
//

#include "VKModel.h"

#include <neon/structure/Application.h>
#include <neon/render/Render.h>
#include <neon/render/shader/Material.h>
#include <neon/render/buffer/CommandBuffer.h>

namespace neon::vulkan {
    std::vector<Mesh::Implementation*> VKModel::getMeshImplementations(
        const std::vector<std::shared_ptr<Mesh>>& meshes) {
        std::vector<Mesh::Implementation*> vector;
        vector.reserve(meshes.size());

        for (auto& item: meshes) {
            vector.push_back(&item->getImplementation());
        }

        return vector;
    }

    VKModel::VKModel(Application* application,
                     const ModelCreateInfo& info,
                     InstanceData* instanceData)
        : _application(application),
          _meshes(getMeshImplementations(info.meshes)),
          _instanceData(instanceData),
          _pipeline(info.pipeline) {}

    void VKModel::draw(const Material* material,
                       const ShaderUniformBuffer* modelBuffer) const {
        auto sp = std::shared_ptr<Material>(const_cast<Material*>(material),
                                            [](Material*) {});

        if (_instanceData->getInstanceAmount() == 0) return;
        for (const auto& mesh: _meshes) {
            if (!mesh->getMaterials().contains(sp)) continue;
            auto* vk = dynamic_cast<AbstractVKApplication*>(
                _application->getImplementation());

            mesh->draw(
                material,
                vk->getCurrentCommandBuffer()
                ->getImplementation().getCommandBuffer(),
                _instanceData->getImplementation().getBuffers(),
                _instanceData->getInstanceAmount(),
                _pipeline,
                &_application->getRender()->getGlobalUniformBuffer(),
                modelBuffer);
        }
    }

    void VKModel::drawOutside(const Material* material,
                              const ShaderUniformBuffer* modelBuffer,
                              const CommandBuffer* commandBuffer) const {
        if (_instanceData->getInstanceAmount() == 0) return;

        auto buffer = commandBuffer->getImplementation().getCommandBuffer();

        vulkan_util::beginRenderPass(buffer, material->getTarget(), true);

        for (const auto& mesh: _meshes) {
            mesh->draw(
                material,
                buffer,
                _instanceData->getImplementation().getBuffers(),
                _instanceData->getInstanceAmount(),
                _pipeline,
                &_application->getRender()->getGlobalUniformBuffer(),
                modelBuffer);
        }

        vkCmdEndRenderPass(buffer);
    }
}
