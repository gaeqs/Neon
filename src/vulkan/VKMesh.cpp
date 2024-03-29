//
// Created by grial on 14/11/22.
//

#include "VKMesh.h"

#include <cstring>
#include <engine/Application.h>
#include <vulkan/VKApplication.h>

namespace neon::vulkan {

    VKMesh::VKMesh(Application* application,
                   std::unordered_set<std::shared_ptr<Material>>& materials,
                   bool modifiableVertices,
                   bool modifiableIndices) :
            _vkApplication(dynamic_cast<AbstractVKApplication*>(
                                   application->getImplementation())),
            _materials(materials),
            _vertexBuffer(),
            _indexBuffer(),
            _verticesSize(0),
            _indexAmount(0),
            _modifiableVertices(modifiableVertices),
            _modifiableIndices(modifiableIndices) {
    }

    const std::unordered_set<std::shared_ptr<Material>>&
    VKMesh::getMaterials() const {
        return _materials;
    }

    void VKMesh::draw(
            const Material* material,
            VkCommandBuffer commandBuffer,
            VkBuffer instancingBuffer,
            uint32_t instancingElements,
            const ShaderUniformBuffer* globalBuffer,
            const ShaderUniformBuffer* modelBuffer) {

        if (!_vertexBuffer.has_value()) return;

        VkBuffer buffers[] = {_vertexBuffer.value()->getRaw(),
                              instancingBuffer};
        VkDeviceSize offsets[] = {0, 0};

        vkCmdBindVertexBuffers(commandBuffer, 0, 2, buffers, offsets);
        vkCmdBindIndexBuffer(
                commandBuffer,
                _indexBuffer.value()->getRaw(),
                0, VK_INDEX_TYPE_UINT32
        );

        auto& mat = material->getImplementation();

        vkCmdBindPipeline(commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          mat.getPipeline());

        mat.uploadConstants(commandBuffer);

        auto layout = mat.getPipelineLayout();

        globalBuffer->getImplementation().bind(commandBuffer, layout);

        if (material->getUniformBuffer() != nullptr) {
            material->getUniformBuffer()
                    ->getImplementation().bind(commandBuffer, layout);
        }

        if (modelBuffer != nullptr) {
            modelBuffer->getImplementation().bind(commandBuffer, layout);
        }

        vkCmdDrawIndexed(commandBuffer, _indexAmount, instancingElements,
                         0, 0, 0);
    }

    bool VKMesh::setVertices(const void* data, size_t length) const {
        if (!_modifiableVertices) return false;
        auto map = _vertexBuffer.value()->map<char>();
        if (!map.has_value()) return false;
        memcpy(map.value()->raw(), data, std::min(length, _verticesSize));
        return true;
    }

    std::vector<uint32_t> VKMesh::getIndices() const {
        if (!_indexBuffer.has_value() || _indexAmount == 0)
            return {};

        auto map = _indexBuffer.value()->map<uint32_t>();
        if (!map.has_value()) return {};

        std::vector<uint32_t> vertices;
        vertices.resize(_indexAmount);

        for (int i = 0; i < _indexAmount; ++i) {
            vertices.push_back(map->get()->get(i));
        }

        return vertices;
    }

    bool VKMesh::setIndices(const std::vector<uint32_t>& indices) const {
        if (!_modifiableIndices) return false;
        auto map = _indexBuffer.value()->map<char>();
        if (!map.has_value()) return false;
        memcpy(map.value()->raw(), indices.data(),
               std::min(indices.size(), static_cast<size_t>(_indexAmount))
               * sizeof(uint32_t));
        return true;
    }
}