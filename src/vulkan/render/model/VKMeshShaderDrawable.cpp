//
// Created by gaeqs on 02/10/2024.
//

#include "VKMeshShaderDrawable.h"

#include <neon/render/model/Model.h>

namespace {
    PFN_vkCmdDrawMeshTasksEXT funVkCmdDrawMeshTasksEXT = nullptr;

    void initFunction(VkInstance instance) {
        if (funVkCmdDrawMeshTasksEXT == nullptr) {
            funVkCmdDrawMeshTasksEXT = reinterpret_cast<
                PFN_vkCmdDrawMeshTasksEXT>(
                vkGetInstanceProcAddr(instance, "vkCmdDrawMeshTasksEXT")
            );
        }
    }

    rush::Vec<3, uint32_t> defaultSupplier(const neon::Model&) {
        return {1, 1, 1};
    }
}

neon::vulkan::VKMeshShaderDrawable::VKMeshShaderDrawable(
    Application* application)
    : _vkApplication(dynamic_cast<AbstractVKApplication*>(application->
          getImplementation())),
      _groupsSupplier(defaultSupplier) {
    initFunction(_vkApplication->getInstance());
}

void neon::vulkan::VKMeshShaderDrawable::draw(
    const Material* material,
    VkCommandBuffer commandBuffer,
    const Model& model,
    const ShaderUniformBuffer* globalBuffer) {
    auto& mat = material->getImplementation();

    vkCmdBindPipeline(commandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      mat.getPipeline());

    mat.uploadConstants(commandBuffer);

    auto layout = mat.getPipelineLayout();
    globalBuffer->getImplementation().bind(commandBuffer, layout, 0);

    if (material->getUniformBuffer() != nullptr) {
        material->getUniformBuffer()->getImplementation().bind(commandBuffer, layout, 1);
    }

    if (auto* modelBuffer = model.getUniformBuffer(); modelBuffer != nullptr) {
        modelBuffer->getImplementation().bind(commandBuffer, layout, 2);
    }

    for (size_t i = 3; auto& buffer: model.getExtraUniformBuffers()) {
        buffer->getImplementation().bind(commandBuffer, layout, i++);
    }

    if (funVkCmdDrawMeshTasksEXT != nullptr) {
        auto groups = _groupsSupplier(model);
        funVkCmdDrawMeshTasksEXT(
            commandBuffer,
            groups.x(),
            groups.y(),
            groups.z()
        );
    }
}

void neon::vulkan::VKMeshShaderDrawable::setGroupsSupplier(
    const std::function<rush::Vec<3, uint32_t>(const Model&)>& supplier) {
    _groupsSupplier = supplier;
}
