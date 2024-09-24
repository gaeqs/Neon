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

neon::vulkan::VKQTSwapChainFrameBuffer::VKQTSwapChainFrameBuffer(
    Application* application) : _qtApplication(dynamic_cast<QTApplication*>(
                                    application->getImplementation())),
                                _renderPass(application,
                                            _qtApplication->
                                            defaultRenderPass()) {
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = _qtApplication->getInstance();
    init_info.PhysicalDevice = _qtApplication->getPhysicalDevice();
    init_info.Device = _qtApplication->getDevice()->getRaw();
    init_info.Queue = _qtApplication->getGraphicsQueue();
    init_info.DescriptorPool = _qtApplication->getImGuiPool();
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, _qtApplication->defaultRenderPass());

    auto cmd = _qtApplication->getCommandPool()->beginCommandBuffer(true);

    ImGui_ImplVulkan_CreateFontsTexture(
        cmd->getImplementation().getCommandBuffer());

    cmd->end();
    cmd->submit();
    cmd->wait();

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

bool neon::vulkan::VKQTSwapChainFrameBuffer::hasDepth() const {
    return true;
}

uint32_t
neon::vulkan::VKQTSwapChainFrameBuffer::getColorAttachmentAmount() const {
    return 1;
}

VkFramebuffer neon::vulkan::VKQTSwapChainFrameBuffer::getRaw() const {
    return _qtApplication->currentFramebuffer();
}

std::vector<VkFormat>
neon::vulkan::VKQTSwapChainFrameBuffer::getColorFormats() const {
    return {_qtApplication->getSwapChainImageFormat()};
}

VkFormat neon::vulkan::VKQTSwapChainFrameBuffer::getDepthFormat() const {
    return _qtApplication->getDepthImageFormat();
}

const neon::vulkan::VKRenderPass&
neon::vulkan::VKQTSwapChainFrameBuffer::getRenderPass() const {
    return _renderPass;
}

neon::vulkan::VKRenderPass&
neon::vulkan::VKQTSwapChainFrameBuffer::getRenderPass() {
    return _renderPass;
}

uint32_t neon::vulkan::VKQTSwapChainFrameBuffer::getWidth() const {
    return _qtApplication->getSwapChainExtent().width;
}

uint32_t neon::vulkan::VKQTSwapChainFrameBuffer::getHeight() const {
    return _qtApplication->getSwapChainExtent().height;
}

bool neon::vulkan::VKQTSwapChainFrameBuffer::renderImGui() {
    return true;
}

#endif