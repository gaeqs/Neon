//
// Created by grial on 14/11/22.
//

#include "VKMesh.h"

#include <cstring>
#include <engine/Application.h>
#include <vulkan/VKApplication.h>

namespace neon::vulkan {
    VKMesh::VKMesh(
        Application* application,
        std::unordered_set<std::shared_ptr<Material>>& materials,
        bool modifiableVertices,
        bool modifiableIndices)
        : _vkApplication(
              dynamic_cast<AbstractVKApplication*>(application->
                  getImplementation())),
          _materials(materials),
          _indexAmount(0),
          _modifiableVertices(
              modifiableVertices),
          _modifiableIndices(
              modifiableIndices) {
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
        constexpr size_t MAX_BUFFERS = 32;

        if (_vertexBuffers.empty()) return;


        VkBuffer buffers[MAX_BUFFERS];
        VkDeviceSize offsets[MAX_BUFFERS];

        size_t i = 0;
        for (; i < std::min(_vertexBuffers.size(), MAX_BUFFERS); ++i) {
            buffers[i] = _vertexBuffers[i]->getRaw();
            offsets[i] = 0;
        }

        if (i < MAX_BUFFERS) {
            buffers[i] = instancingBuffer;
            offsets[i] = 0;
            ++i;
        }

        vkCmdBindVertexBuffers(
            commandBuffer,
            0,
            i,
            buffers,
            offsets
        );
        vkCmdBindIndexBuffer(
            commandBuffer,
            _indexBuffer.value()->getRaw(),
            0,
            VK_INDEX_TYPE_UINT32
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

        vkCmdDrawIndexed(
            commandBuffer,
            _indexAmount,
            instancingElements,
            0,
            0,
            0
        );
    }

    bool VKMesh::setVertices(size_t index,
                             const void* data,
                             size_t length,
                             CommandBuffer* cmd) const {
        if (!_modifiableVertices) return false;
        auto map = _vertexBuffers.at(index)->map<char>(cmd);
        if (!map.has_value()) return false;
        memcpy(map.value()->raw(), data, std::min(length, _vertexSizes[index]));
        return true;
    }

    std::vector<uint32_t> VKMesh::getIndices(CommandBuffer* cmd) const {
        if (!_indexBuffer.has_value() || _indexAmount == 0)
            return {};

        auto map = _indexBuffer.value()->map<uint32_t>(cmd);
        if (!map.has_value()) return {};

        std::vector<uint32_t> vertices;
        vertices.resize(_indexAmount);

        for (int i = 0; i < _indexAmount; ++i) {
            vertices.push_back(map->get()->get(i));
        }

        return vertices;
    }

    bool VKMesh::setIndices(const std::vector<uint32_t>& indices,
                            CommandBuffer* cmd) const {
        if (!_modifiableIndices) return false;
        auto map = _indexBuffer.value()->map<char>(cmd);
        if (!map.has_value()) return false;
        memcpy(map.value()->raw(), indices.data(),
               std::min(indices.size(), static_cast<size_t>(_indexAmount))
               * sizeof(uint32_t));
        return true;
    }
}
