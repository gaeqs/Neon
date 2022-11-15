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

class VKMesh {

    VKApplication* _vkApplication;

    VkPipelineLayout _pipelineLayout;
    VkRenderPass _renderPass;
    VkPipeline _pipeline;

    const IdentifiableCollection<ShaderUniformBuffer>& _uniforms;


    void createGraphicPipeline(
            const VkVertexInputBindingDescription& bindingDescription,
            const std::vector<VkVertexInputAttributeDescription>&
            attributeDescriptions,
            const std::vector<std::shared_ptr<GLShaderProgram>>& shaders);

    void destroyGraphicPipeline();

public:

    VKMesh(Application* application,
           const IdentifiableCollection<ShaderUniformBuffer>& uniforms,
           Material& material);

    ~VKMesh();

    template<class Vertex>
    void uploadData(const std::vector<Vertex>& vertices,
                    const std::vector<uint32_t>& indices) {

        std::vector<VkVertexInputAttributeDescription> attributeDescriptions =
                Vertex::getAttributeDescriptions();

        VkVertexInputBindingDescription bindingDescription =
                Vertex::getBindingDescription();

        destroyGraphicPipeline();
        createGraphicPipeline(
                bindingDescription,
                attributeDescriptions,

        );
    }

};


#endif //NEON_VKMESH_H
