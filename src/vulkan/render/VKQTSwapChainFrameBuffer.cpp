//
// Created by gaelr on 10/06/23.
//

#ifdef USE_QT

    #include "VKQTSwapChainFrameBuffer.h"

    #include <vulkan/QTApplication.h>

    #include <vulkan/vulkan.h>

    #include <imgui_impl_vulkan.h>
    #include <neon/structure/Application.h>

    #include <vulkan/util/VKUtil.h>

    #include <neon/render/buffer/CommandBuffer.h>
    #include <vulkan/util/VulkanConversions.h>

namespace neon::vulkan
{
    VKQTSwapChainFrameBuffer::VKQTSwapChainFrameBuffer(Application* application) :
        VKFrameBuffer(application),
        _qtApplication(dynamic_cast<QTApplication*>(application->getImplementation())),
        _renderPass(application, _qtApplication->defaultRenderPass())
    {
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = _qtApplication->getInstance();
        init_info.PhysicalDevice = _qtApplication->getPhysicalDevice().getRaw();
        init_info.Device = _qtApplication->getDevice()->getRaw();
        init_info.Queue = _qtApplication->getGraphicsQueue();
        init_info.DescriptorPool = _qtApplication->getImGuiPool();
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = conversions::vkSampleCountFlagBits(SamplesPerTexel::COUNT_1);
        init_info.RenderPass = _renderPass.getRaw();

        ImGui_ImplVulkan_Init(&init_info);
    }

    bool VKQTSwapChainFrameBuffer::hasDepth() const
    {
        return true;
    }

    uint32_t VKQTSwapChainFrameBuffer::getColorAttachmentAmount() const
    {
        return 1;
    }

    std::vector<VkFormat> VKQTSwapChainFrameBuffer::getColorFormats() const
    {
        return {_qtApplication->getSwapChainImageFormat()};
    }

    const VKRenderPass& VKQTSwapChainFrameBuffer::getRenderPass() const
    {
        return _renderPass;
    }

    VKRenderPass& VKQTSwapChainFrameBuffer::getRenderPass()
    {
        return _renderPass;
    }

    bool VKQTSwapChainFrameBuffer::renderImGui()
    {
        return true;
    }

    rush::Vec2ui VKQTSwapChainFrameBuffer::getDimensions() const
    {
        auto [width, height] = _qtApplication->getSwapChainExtent();
        return {width, height};
    }

    SamplesPerTexel VKQTSwapChainFrameBuffer::getSamples() const
    {
        return SamplesPerTexel::COUNT_1;
    }

    std::vector<FrameBufferOutput> VKQTSwapChainFrameBuffer::getOutputs() const
    {
        FrameBufferOutput output;
        output.type = FrameBufferOutputType::SWAP;
        return {output};
    }

    void* VKQTSwapChainFrameBuffer::getNativeHandle()
    {
        return _qtApplication->currentFramebuffer();
    }

    const void* VKQTSwapChainFrameBuffer::getNativeHandle() const
    {
        return _qtApplication->currentFramebuffer();
    }

    std::optional<VkFormat> VKQTSwapChainFrameBuffer::getDepthFormat() const
    {
        return _qtApplication->getVkDepthImageFormat();
    }
} // namespace neon::vulkan

#endif