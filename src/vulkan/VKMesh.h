//
// Created by grial on 14/11/22.
//

#ifndef NEON_VKMESH_H
#define NEON_VKMESH_H

#include <type_traits>

#include <engine/Material.h>
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
    Material& _material;

    VkPipelineLayout _pipelineLayout;
    VkRenderPass _renderPass;
    VkPipeline _pipeline;

    std::optional<std::unique_ptr<SimpleBuffer>> _vertexBuffer;
    std::optional<std::unique_ptr<SimpleBuffer>> _indexBuffer;

    std::vector<VkVertexInputAttributeDescription> _attributeDescriptions;
    std::vector<VkVertexInputAttributeDescription> _instancingAttributeDescriptions;
    VkVertexInputBindingDescription _bindingDescription;
    VkVertexInputBindingDescription _instancingBindingDescription;

    std::string _currentShader;
    uint64_t _currentUniformStatus;

    bool _dirty;

    void createGraphicPipeline(
            VKShaderProgram* shader,
            const IdentifiableCollection<ShaderUniformBuffer>& uniforms);

    void destroyGraphicPipeline();

public:

    VKMesh(Application* application, Material& material);

    ~VKMesh();

    template<class InstancingData>
    void configureInstancingBuffer() {
        _instancingAttributeDescriptions =
                InstancingData::getInstancingAttributeDescriptions();
        _instancingBindingDescription =
                InstancingData::getInstancingBindingDescription();
        _dirty = true;
    }

    template<class Vertex>
    void uploadData(const std::vector<Vertex>& vertices,
                    const std::vector<uint32_t>& indices) {

        _attributeDescriptions = Vertex::getAttributeDescriptions();
        _bindingDescription = Vertex::getBindingDescription();

        _vertexBuffer = std::make_unique<SimpleBuffer>(
                _vkApplication->getPhysicalDevice(),
                _vkApplication->getDevice(),
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                vertices);

        _indexBuffer = std::make_unique<SimpleBuffer>(
                _vkApplication->getPhysicalDevice(),
                _vkApplication->getDevice(),
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                indices);

        _dirty = true;
    }

    void draw(
            VkCommandBuffer commandBuffer,
            VKShaderProgram* shader,
            VkBuffer instancingBuffer,
            uint32_t instancingElements,
            const IdentifiableCollection<ShaderUniformBuffer>& uniforms);

};


#endif //NEON_VKMESH_H
