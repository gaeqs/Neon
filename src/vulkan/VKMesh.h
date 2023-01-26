//
// Created by grial on 14/11/22.
//

#ifndef NEON_VKMESH_H
#define NEON_VKMESH_H

#include <type_traits>
#include <vector>

#include <engine/shader/Material.h>
#include <vulkan/vulkan.h>
#include <engine/collection/IdentifiableCollection.h>
#include <engine/shader/ShaderUniformBuffer.h>
#include <vulkan/VKApplication.h>
#include <vulkan/buffer/SimpleBuffer.h>

class Application;

class VKApplication;

class VKShaderProgram;

class VKMesh {

    VKApplication* _vkApplication;
    IdentifiableWrapper<Material>& _material;

    std::optional<std::unique_ptr<SimpleBuffer>> _vertexBuffer;
    std::optional<std::unique_ptr<SimpleBuffer>> _indexBuffer;
    uint32_t _indexAmount;

public:

    VKMesh(const VKMesh& other) = delete;

    VKMesh(Application* application, IdentifiableWrapper<Material>& material);

    template<class Vertex>
    void uploadData(const std::vector<Vertex>& vertices,
                    const std::vector<uint32_t>& indices) {

        _vertexBuffer = std::make_unique<SimpleBuffer>(
                _vkApplication,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                vertices);

        _indexBuffer = std::make_unique<SimpleBuffer>(
                _vkApplication,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                indices);

        _indexAmount = indices.size();
    }

    IdentifiableWrapper<Material> getMaterial() const;

    void draw(
            VkCommandBuffer commandBuffer,
            VkBuffer instancingBuffer,
            uint32_t instancingElements,
            const ShaderUniformBuffer* global);

};


#endif //NEON_VKMESH_H
