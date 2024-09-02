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

#include <vulkan/AbstractVKApplication.h>
#include <vulkan/buffer/SimpleBuffer.h>
#include <vulkan/buffer/StagingBuffer.h>

namespace neon {
    class Application;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKShaderProgram;

    class VKMesh {
        AbstractVKApplication* _vkApplication;
        std::unordered_set<std::shared_ptr<Material>>& _materials;

        std::vector<std::unique_ptr<Buffer>> _vertexBuffers;
        std::vector<size_t> _vertexSizes;

        std::optional<std::unique_ptr<Buffer>> _indexBuffer;
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

        template<typename... Types>
        void uploadVertices(const std::vector<Types>&... data) {
            _vertexBuffers.clear();
            _vertexSizes.clear();
            _vertexBuffers.reserve(sizeof...(data));
            if (_modifiableVertices) {
                ([&] {
                    _vertexBuffers.push_back(
                        std::make_unique<StagingBuffer>(
                            _vkApplication,
                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            data
                        )
                    );
                    _vertexSizes.push_back(data.size() * sizeof(Types));
                }(), ...);
            }
            else {
                ([&] {
                    _vertexBuffers.push_back(
                        std::make_unique<SimpleBuffer>(
                            _vkApplication,
                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                            data
                        )
                    );
                    _vertexSizes.push_back(data.size() * sizeof(Types));
                }(), ...);
            }
        }

        void uploadIndices(const std::vector<uint32_t>& indices) {
            if (_modifiableIndices) {
                _indexBuffer = std::make_unique<StagingBuffer>(
                    _vkApplication,
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                    indices);
            }
            else {
                _indexBuffer = std::make_unique<SimpleBuffer>(
                    _vkApplication,
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                    indices);
            }

            _indexAmount = indices.size();
        }

        template<class Vertex>
        std::vector<Vertex> getVertices(size_t index,
                                        CommandBuffer* cmd = nullptr) const {
            if (!_vertexBuffers.size() <= index
                || _vertexSizes[index] == 0
                || sizeof(Vertex) == 0)
                return {};

            auto map = _vertexBuffers.at(index)->map<Vertex>(cmd);
            if (!map.has_value()) return {};

            size_t amount = _vertexSizes[index] / sizeof(Vertex);

            std::vector<Vertex> vertices;
            vertices.resize(amount);

            for (int i = 0; i < amount; ++i) {
                vertices.push_back(map.value().get()->get(i));
            }

            return vertices;
        }

        template<class Vertex>
        bool setVertices(size_t index,
                         const std::vector<Vertex>& vertices,
                         CommandBuffer* cmd = nullptr) const {

            return setVertices(
                index,
                vertices.data(),
                vertices.size() * sizeof(Vertex),
                cmd
            );
        }

        bool setVertices(size_t index,
                         const void* data,
                         size_t length,
                         CommandBuffer* cmd = nullptr) const;

        [[nodiscard]] std::vector<uint32_t>
        getIndices(CommandBuffer* cmd = nullptr) const;

        bool setIndices(const std::vector<uint32_t>& indices,
                        CommandBuffer* cmd = nullptr) const;

        [[nodiscard]] const std::unordered_set<std::shared_ptr<Material>>&
        getMaterials() const;

        void draw(
            const Material* material,
            VkCommandBuffer commandBuffer,
            const std::vector<std::unique_ptr<Buffer>>& instancingBuffers,
            uint32_t instancingElements,
            const ShaderUniformBuffer* globalBuffer,
            const ShaderUniformBuffer* modelBuffer);
    };
}

#endif //NEON_VKMESH_H
