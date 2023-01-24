//
// Created by grial on 22/11/22.
//

#ifndef NEON_VKMATERIAL_H
#define NEON_VKMATERIAL_H

#include <vector>
#include <unordered_map>
#include <string>

#include <vulkan/vulkan.h>

#include <engine/structure/IdentifiableWrapper.h>
#include <engine/render/Texture.h>
#include <engine/model/InputDescription.h>

class Application;

class VKApplication;

class Room;

class Material;

class FrameBuffer;

class VKMaterial {

    Material* _material;

    VKApplication* _vkApplication;
    VkPipelineLayout _pipelineLayout;
    VkPipeline _pipeline;

    std::vector<char> _pushConstants;
    VkShaderStageFlags _pushConstantStages;

    VkRenderPass _target;

public:

    VKMaterial(const VKMaterial& other) = delete;

    VKMaterial(Room* room, Material* material,
               const std::shared_ptr<FrameBuffer>& target,
               const InputDescription& vertexDescription,
               const InputDescription& instanceDescription);

    ~VKMaterial();

    [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

    [[nodiscard]] VkPipeline getPipeline() const;

    [[nodiscard]] VkRenderPass getTarget() const;

    void pushConstant(const std::string& name, const void* data, uint32_t size);

    void uploadConstants(VkCommandBuffer buffer) const;

    void setTexture(const std::string& name,
                    IdentifiableWrapper<Texture> texture);
};


#endif //NEON_VKMATERIAL_H
