//
// Created by grial on 14/11/22.
//

#include "VKMesh.h"

#include <cstring>
#include <neon/render/model/Model.h>
#include <neon/structure/Application.h>
#include <vulkan/VKApplication.h>

namespace neon::vulkan {
    VKMesh::VKMesh(
        Application* application,
        bool modifiableVertices,
        bool modifiableIndices)
        : _vkApplication(
              dynamic_cast<AbstractVKApplication*>(application->
                  getImplementation())),
          _indexAmount(0),
          _modifiableVertices(modifiableVertices),
          _modifiableIndices(modifiableIndices) {}

    void VKMesh::draw(
        const Material* material,
        VkCommandBuffer commandBuffer,
        const Model& model,
        const ShaderUniformBuffer* globalBuffer) {
        constexpr size_t MAX_BUFFERS = 32;

        if (_vertexBuffers.empty()) return;


        VkBuffer buffers[MAX_BUFFERS];
        VkDeviceSize offsets[MAX_BUFFERS];

        size_t i = 0;
        for (; i < std::min(_vertexBuffers.size(), MAX_BUFFERS); ++i) {
            buffers[i] = _vertexBuffers[i]->getRaw();
            offsets[i] = 0;
        }

        size_t instances = model.getInstanceDatas().empty() ? 1 : model.getInstanceData(0)->getInstanceAmount();
        for (auto& data: model.getInstanceDatas()) {
            auto& instancingBuffers = data->getImplementation().getBuffers();
            for (size_t j = 0; j < instancingBuffers.size() && i < MAX_BUFFERS; ++j, ++i) {
                auto& buffer = instancingBuffers[j];
                buffers[i] = buffer == nullptr ? VK_NULL_HANDLE : buffer->getRaw();
                offsets[i] = 0;
            }
            instances = std::min(instances, data->getInstanceAmount());
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

        for (auto [binding, entry]: model.getUniformBufferBindings()) {
            switch (entry.location) {
                case ModelBufferLocation::GLOBAL:
                    if (globalBuffer != nullptr) {
                        globalBuffer->getImplementation().bind(commandBuffer, layout, binding);
                    }
                    break;
                case ModelBufferLocation::MATERIAL:
                    if (auto& buffer = material->getUniformBuffer(); buffer != nullptr) {
                        material->getUniformBuffer()->getImplementation().bind(commandBuffer, layout, binding);
                    }
                    break;
                case ModelBufferLocation::MODEL:
                    if (auto buffer = model.getUniformBuffer(); buffer != nullptr) {
                        material->getUniformBuffer()->getImplementation().bind(commandBuffer, layout, binding);
                    }
                    break;
                case ModelBufferLocation::EXTRA:
                    if (entry.extraBuffer != nullptr) {
                        entry.extraBuffer->getImplementation().bind(commandBuffer, layout, binding);
                    }
                    break;
            }
        }

        vkCmdDrawIndexed(
            commandBuffer,
            _indexAmount,
            instances,
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
