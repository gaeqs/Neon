//
// Created by gaeqs on 02/10/2024.
//

#include "VKMeshShaderDrawable.h"

#include <neon/render/model/Model.h>

namespace
{
    PFN_vkCmdDrawMeshTasksEXT funVkCmdDrawMeshTasksEXT = nullptr;

    void initFunction(VkInstance instance)
    {
        if (funVkCmdDrawMeshTasksEXT == nullptr) {
            funVkCmdDrawMeshTasksEXT =
                reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(vkGetInstanceProcAddr(instance, "vkCmdDrawMeshTasksEXT"));
        }
    }

    rush::Vec<3, uint32_t> defaultSupplier(const neon::Model&)
    {
        return {1, 1, 1};
    }
} // namespace

neon::vulkan::VKMeshShaderDrawable::VKMeshShaderDrawable(Application* application) :
    _vkApplication(dynamic_cast<AbstractVKApplication*>(application->getImplementation())),
    _groupsSupplier(defaultSupplier)
{
    initFunction(_vkApplication->getInstance());
}

void neon::vulkan::VKMeshShaderDrawable::draw(Material* material, VKCommandBuffer* commandBuffer, const Model& model,
                                              ShaderUniformBuffer* globalBuffer)
{
    auto rawCmd = commandBuffer->getCommandBuffer();
    auto& mat = material->getImplementation();

    vkCmdBindPipeline(rawCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mat.getPipeline());

    mat.useMaterial(commandBuffer->getCurrentRun());
    mat.uploadConstants(rawCmd);

    auto layout = mat.getPipelineLayout();

    for (auto [binding, entry] : model.getUniformBufferBindings()) {
        switch (entry.location) {
            case ModelBufferLocation::GLOBAL:
                if (globalBuffer != nullptr) {
                    globalBuffer->getImplementation().bind(commandBuffer, layout, binding);
                }
                break;
            case ModelBufferLocation::MATERIAL:
                if (auto& buffer = material->getUniformBuffer(); buffer != nullptr) {
                    buffer->getImplementation().bind(commandBuffer, layout, binding);
                }
                break;
            case ModelBufferLocation::MODEL:
                if (auto buffer = model.getUniformBuffer(); buffer != nullptr) {
                    buffer->getImplementation().bind(commandBuffer, layout, binding);
                }
                break;
            case ModelBufferLocation::EXTRA:
                if (entry.extraBuffer != nullptr) {
                    entry.extraBuffer->getImplementation().bind(commandBuffer, layout, binding);
                }
                break;
        }
    }

    if (funVkCmdDrawMeshTasksEXT != nullptr) {
        auto groups = _groupsSupplier(model);
        funVkCmdDrawMeshTasksEXT(rawCmd, groups.x(), groups.y(), groups.z());
    }
}

void neon::vulkan::VKMeshShaderDrawable::setGroupsSupplier(
    const std::function<rush::Vec<3, uint32_t>(const Model&)>& supplier)
{
    _groupsSupplier = supplier;
}
