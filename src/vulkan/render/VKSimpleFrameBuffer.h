//
// Created by grial on 15/12/22.
//

#ifndef NEON_VKSIMPLEFRAMEBUFFER_H
#define NEON_VKSIMPLEFRAMEBUFFER_H

#include <vector>

#include <engine/render/TextureFormat.h>
#include <engine/structure/IdentifiableWrapper.h>

#include <vulkan/render/VKFrameBuffer.h>
#include <vulkan/render/VKRenderPass.h>
#include <imgui.h>

class Room;

class VKApplication;

class Texture;

class VKSimpleFrameBuffer : public VKFrameBuffer {

    VKApplication* _vkApplication;

    VkFramebuffer _frameBuffer;

    std::vector<VkImage> _images;
    std::vector<VkDeviceMemory> _memories;
    std::vector<VkImageView> _imageViews;
    std::vector<VkImageLayout> _layouts;
    std::vector<VkDescriptorSet> _imGuiDescriptors;
    std::vector<IdentifiableWrapper<Texture>> _textures;

    std::vector<VkFormat> _formats;
    VKRenderPass _renderPass;

    bool _depth;

    void createImages();

    void createFrameBuffer();

    void cleanup();


public:

    VKSimpleFrameBuffer(Room* room,
                        const std::vector<TextureFormat>& formats,
                        bool depth);

    ~VKSimpleFrameBuffer() override;

    [[nodiscard]] VkFramebuffer getRaw() const override;

    [[nodiscard]] bool hasDepth() const override;

    [[nodiscard]] uint32_t getColorAttachmentAmount() const override;

    [[nodiscard]] std::vector<VkFormat> getColorFormats() const override;

    [[nodiscard]] VkFormat getDepthFormat() const override;

    [[nodiscard]] VKRenderPass const& getRenderPass() const override;

    [[nodiscard]] VKRenderPass& getRenderPass() override;

    [[nodiscard]] uint32_t getWidth() const override;

    [[nodiscard]] uint32_t getHeight() const override;

    [[nodiscard]] ImTextureID getImGuiDescriptor(uint32_t index);

    bool renderImGui() override;

    [[nodiscard]] const std::vector<IdentifiableWrapper<Texture>>&
    getTextures() const;

    void recreate();

};


#endif //NEON_VKSIMPLEFRAMEBUFFER_H
