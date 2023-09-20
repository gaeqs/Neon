//
// Created by gaelr on 10/06/23.
//

#include "VKQTSwapChainFrameBuffer.h"

#include <vulkan/QTApplication.h>

#include <vulkan/vulkan.h>

neon::vulkan::VKQTSwapChainFrameBuffer::VKQTSwapChainFrameBuffer(
        Application* application ) :
        _qtApplication(dynamic_cast<QTApplication*>(
                               application->getImplementation())),
        _renderPass(application, _qtApplication->defaultRenderPass()) {

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
    return false;
}
