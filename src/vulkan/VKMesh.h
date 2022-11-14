//
// Created by grial on 14/11/22.
//

#ifndef NEON_VKMESH_H
#define NEON_VKMESH_H

#include <engine/Material.h>
#include <vulkan/vulkan.h>

class Application;

class VKApplication;

class VKMesh {

    VKApplication* _vkApplication;


    void createGraphicPipeline(
            const VkVertexInputBindingDescription& bindingDescription,
            const std::vector<VkVertexInputAttributeDescription>&
            attributeDescriptions,
            const std::vector<std::shared_ptr<Shader>>& shaders);

public:

    VKMesh(Application* application, Material& material);

    template<class Vertex>
    void uploadData(std::vector<Vertex> vertices,
                    std::vector<uint32_t> indices) {

        std::vector<VkVertexInputAttributeDescription> attributeDescriptions =
                Vertex::getAttributeDescriptions();

        VkVertexInputBindingDescription bindingDescription =
                Vertex::getBindingDescription();


    }

};


#endif //NEON_VKMESH_H
