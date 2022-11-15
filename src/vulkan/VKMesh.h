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

class Application;

class VKApplication;

class VKShaderProgram;

class VKMesh {

    VKApplication* _vkApplication;

    VkPipelineLayout _pipelineLayout;
    VkRenderPass _renderPass;
    VkPipeline _pipeline;

    std::vector<VkVertexInputAttributeDescription> _attributeDescriptions;
    VkVertexInputBindingDescription _bindingDescription;

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

    template<class Vertex>
    void uploadData(const std::vector<Vertex>& vertices,
                    const std::vector<uint32_t>& indices) {

        _attributeDescriptions = Vertex::getAttributeDescriptions();
        _bindingDescription = Vertex::getBindingDescription();

        _dirty = true;
    }

    void draw(
            const std::string& shaderName,
            VKShaderProgram* shader,
            const IdentifiableCollection<ShaderUniformBuffer>& uniforms);

};


#endif //NEON_VKMESH_H
