//
// Created by gaelr on 10/06/23.
//

#include "VKQTSwapChainFrameBuffer.h"

#include <vulkan/QTApplication.h>

#include <vulkan/vulkan.h>

#include <imgui_impl_vulkan.h>
#include <engine/Application.h>

#include <vulkan/util/VKUtil.h>

neon::vulkan::VKQTSwapChainFrameBuffer::VKQTSwapChainFrameBuffer(
        Application* application ) :
        _qtApplication(dynamic_cast<QTApplication*>(
                               application->getImplementation())),
        _renderPass(application, _qtApplication->defaultRenderPass()) {

    auto* app = dynamic_cast<vulkan::QTApplication*>(
            application->getImplementation());

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = _qtApplication->getInstance();
    init_info.PhysicalDevice = _qtApplication->getPhysicalDevice();
    init_info.Device = _qtApplication->getDevice();
    init_info.Queue = _qtApplication->getGraphicsQueue();
    init_info.DescriptorPool = _qtApplication->getImGuiPool();
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, _qtApplication->defaultRenderPass());

    auto cmd = vulkan::vulkan_util::beginSingleTimeCommandBuffer(
            _qtApplication->getDevice(), _qtApplication->getCommandPool());
    ImGui_ImplVulkan_CreateFontsTexture(cmd);
    vulkan::vulkan_util::endSingleTimeCommandBuffer(
            app->getDevice(), app->getGraphicsQueue(),
            app->getCommandPool(), cmd);

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

bool neon::vulkan::VKQTSwapChainFrameBuffer::hasDepth( ) const {
    return true;
}

uint32_t
neon::vulkan::VKQTSwapChainFrameBuffer::getColorAttachmentAmount( ) const {
    return 1;
}

VkFramebuffer neon::vulkan::VKQTSwapChainFrameBuffer::getRaw( ) const {
    return _qtApplication->currentFramebuffer();
}

std::vector<VkFormat>
neon::vulkan::VKQTSwapChainFrameBuffer::getColorFormats( ) const {
    return {_qtApplication->getSwapChainImageFormat()};
}

VkFormat neon::vulkan::VKQTSwapChainFrameBuffer::getDepthFormat( ) const {
    return _qtApplication->getDepthImageFormat();
}

const neon::vulkan::VKRenderPass&
neon::vulkan::VKQTSwapChainFrameBuffer::getRenderPass( ) const {
    return _renderPass;
}

neon::vulkan::VKRenderPass&
neon::vulkan::VKQTSwapChainFrameBuffer::getRenderPass( ) {
    return _renderPass;
}

uint32_t neon::vulkan::VKQTSwapChainFrameBuffer::getWidth( ) const {
    return _qtApplication->getSwapChainExtent().width;
}

uint32_t neon::vulkan::VKQTSwapChainFrameBuffer::getHeight( ) const {
    return _qtApplication->getSwapChainExtent().height;
}

bool neon::vulkan::VKQTSwapChainFrameBuffer::renderImGui( ) {
    return true;
}
