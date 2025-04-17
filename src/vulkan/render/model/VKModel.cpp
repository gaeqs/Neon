//
// Created by gaelr on 15/11/2022.
//

#include "VKModel.h"

#include <neon/structure/Application.h>
#include <neon/render/Render.h>
#include <neon/render/shader/Material.h>
#include <neon/render/buffer/CommandBuffer.h>
#include <neon/render/model/Model.h>

namespace neon::vulkan
{
    VKModel::VKModel(Application* application, Model* model) :
        _application(application),
        _model(model)
    {
    }

    void VKModel::draw(Material* material) const
    {
        auto sp = std::shared_ptr<Material>(const_cast<Material*>(material), [](Material*) {});

        for (const auto& mesh : _model->getMeshes()) {
            if (!mesh->getMaterials().contains(sp)) {
                continue;
            }
            auto* vk = dynamic_cast<AbstractVKApplication*>(_application->getImplementation());

            mesh->getImplementation().draw(material, &vk->getCurrentCommandBuffer()->getImplementation(), *_model, &_application->getRender()->getGlobalUniformBuffer());
        }
    }

    void VKModel::drawOutside(Material* material, CommandBuffer* commandBuffer) const
    {
        auto& buffer = commandBuffer->getImplementation();

        vulkan_util::beginRenderPass(&buffer, material->getTarget(), true);

        for (const auto& mesh : _model->getMeshes()) {
            mesh->getImplementation().draw(material, &buffer, *_model,
                                           &_application->getRender()->getGlobalUniformBuffer());
        }

        vkCmdEndRenderPass(buffer.getCommandBuffer());
    }
} // namespace neon::vulkan
