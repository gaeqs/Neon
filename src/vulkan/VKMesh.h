//
// Created by grial on 14/11/22.
//

#ifndef NEON_VKMESH_H
#define NEON_VKMESH_H

#include <cstdint>
#include <type_traits>
#include <vector>
#include <unordered_set>
#include <optional>
#include <memory>

#include <vulkan/vulkan.h>

#include <engine/shader/Material.h>
#include <engine/shader/ShaderUniformBuffer.h>

#include <vulkan/VKApplication.h>
#include <vulkan/buffer/SimpleBuffer.h>
#include <vulkan/buffer/StagingBuffer.h>

namespace neon {
    class Application;
}

namespace neon::vulkan {

    class VKApplication;

    class VKShaderProgram;

    class VKMesh {

        VKApplication* _vkApplication;
        std::unordered_set<std::shared_ptr<Material>>& _materials;

        std::optional<std::unique_ptr<Buffer>> _vertexBuffer;
        std::optional<std::unique_ptr<Buffer>> _indexBuffer;

        size_t _verticesSize;
        uint32_t _indexAmount;

        bool _modifiableVertices;
        bool _modifiableIndices;

    public:

        VKMesh(const VKMesh& other) = delete;

        VKMesh(Application* application,
               std::unordered_set<std::shared_ptr<Material>>& materials,
               bool modifiableVertices,
               bool modifiableIndices
        );

        template<class Vertex>
        void uploadData(const std::vector<Vertex>& vertices,
                        const std::vector<uint32_t>& indices) {

            if (_modifiableVertices) {
                _vertexBuffer = std::make_unique<StagingBuffer>(
                        _vkApplication,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        vertices);
            } else {
                _vertexBuffer = std::make_unique<SimpleBuffer>(
                        _vkApplication,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                        vertices);
            }

            if (_modifiableIndices) {
                _indexBuffer = std::make_unique<StagingBuffer>(
                        _vkApplication,
                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        indices);
            } else {
                _indexBuffer = std::make_unique<SimpleBuffer>(
                        _vkApplication,
                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                        indices);
            }

            _indexAmount = indices.size();
            _verticesSize = vertices.size() * sizeof(Vertex);
        }

        template<class Vertex>
        std::vector<Vertex> getVertices() const {
            if (!_vertexBuffer.has_value()
                || _verticesSize == 0
                || sizeof(Vertex) == 0)
                return {};

            auto map = _vertexBuffer.value()->map<Vertex>();
            if (!map.has_value()) return {};

            size_t amount = _verticesSize / sizeof(Vertex);

            std::vector<Vertex> vertices;
            vertices.resize(amount);

            for (int i = 0; i < amount; ++i) {
                vertices.push_back(map[i]);
            }

            return vertices;
        }

        template<class Vertex>
        bool setVertices(const std::vector<Vertex>& vertices) const {
            return setVertices(vertices.data(),
                               vertices.size() * sizeof(Vertex));
        }

        bool setVertices(const void* data, size_t length) const;

        [[nodiscard]] std::vector<uint32_t> getIndices() const;

        bool setIndices(const std::vector<uint32_t>& indices) const;

        [[nodiscard]] const std::unordered_set<std::shared_ptr<Material>>&
        getMaterials() const;

        void draw(
                const std::shared_ptr<Material>& material,
                VkCommandBuffer commandBuffer,
                VkBuffer instancingBuffer,
                uint32_t instancingElements,
                const ShaderUniformBuffer* global);

    };
}

#endif //NEON_VKMESH_H
