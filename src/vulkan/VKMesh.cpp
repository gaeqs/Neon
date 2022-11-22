//
// Created by grial on 14/11/22.
//

#include "VKMesh.h"

#include <engine/Application.h>
#include <vulkan/VKApplication.h>

VKMesh::VKMesh(Application* application, Material& material) :
        _vkApplication(&application->getImplementation()),
        _material(material),
        _vertexBuffer(),
        _indexBuffer(),
        _indexAmount(0) {
}

void VKMesh::draw(
        VkCommandBuffer commandBuffer,
        VkBuffer instancingBuffer,
        uint32_t instancingElements,
        const std::vector<const ShaderUniformBuffer*>& uniforms) {

    if (!_vertexBuffer.has_value()) return;

    VkBuffer buffers[] = {_vertexBuffer.value()->getRaw(), instancingBuffer};
    VkDeviceSize offsets[] = {0, 0};

    auto& mat = _material.getImplementation();

    vkCmdBindPipeline(commandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      mat.getPipeline());
    vkCmdBindVertexBuffers(commandBuffer, 0, 2, buffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer,
                         _indexBuffer.value()->getRaw(),
                         0, VK_INDEX_TYPE_UINT32);

    auto layout = mat.getPipelineLayout();
    for (const auto& uniform: uniforms) {
        uniform->getImplementation().bind(commandBuffer, layout);
    }

    vkCmdDrawIndexed(commandBuffer, _indexAmount, instancingElements, 0, 0, 0);
}
