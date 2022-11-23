//
// Created by grial on 22/11/22.
//

#ifndef NEON_VKMATERIAL_H
#define NEON_VKMATERIAL_H

#include <vector>
#include <unordered_map>
#include <string>

#include <vulkan/vulkan.h>

#include <engine/model/InputDescription.h>

class Application;

class VKApplication;

class Room;

class Material;

class VKMaterial {

    Material* _material;

    VKApplication* _vkApplication;
    VkPipelineLayout _pipelineLayout;
    VkPipeline _pipeline;

    std::vector<char> _pushConstants;
    VkShaderStageFlags _pushConstantStages;

public:

    VKMaterial(const VKMaterial& other) = delete;

    VKMaterial(Room* room, Material* material,
               const InputDescription& vertexDescription,
               const InputDescription& instanceDescription);

    ~VKMaterial();

    [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

    [[nodiscard]] VkPipeline getPipeline() const;

    void pushConstant(const std::string& name, const void* data, uint32_t size);

    void uploadConstants(VkCommandBuffer buffer) const;
};


#endif //NEON_VKMATERIAL_H
