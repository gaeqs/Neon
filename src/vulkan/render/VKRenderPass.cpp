//
// Created by grial on 15/12/22.
//

#include "VKRenderPass.h"

#include <stdexcept>

#include <neon/structure/Application.h>

#include <vulkan/render/VKFrameBuffer.h>
#include <vulkan/AbstractVKApplication.h>
#include <vulkan/util/VulkanConversions.h>

namespace neon::vulkan {
    VKRenderPass::VKRenderPass(VKRenderPass&& other) noexcept:
        _vkApplication(other._vkApplication),
        _raw(other._raw),
        _external(other._external) {
        other._raw = VK_NULL_HANDLE;
    }

    VKRenderPass::VKRenderPass(Application* application,
                               const std::vector<VkFormat>& colorFormats,
                               const std::vector<VkSampleCountFlagBits>& samples,
                               bool depth,
                               bool present,
                               bool resolve,
                               VkFormat depthFormat,
                               VkSampleCountFlagBits depthSamples) :
        _vkApplication(dynamic_cast<AbstractVKApplication*>(
            application->getImplementation())),
        _raw(VK_NULL_HANDLE),
        _external(false) {
        std::vector<VkAttachmentDescription> attachments;
        attachments.reserve(colorFormats.size() * 2 + (depth ? 1 : 0));

        std::vector<VkAttachmentReference> references;
        references.reserve(colorFormats.size());

        std::vector<VkAttachmentReference> resolveReferences;
        references.reserve(colorFormats.size());

        uint32_t sampleIndex = 0;
        for (const auto& format: colorFormats) {
            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = format;
            colorAttachment.samples = samples[sampleIndex++];
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            if (resolve) {
                colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            } else {
                if (present) {
                    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                } else {
                    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                }
            }

            attachments.push_back(colorAttachment);

            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = attachments.size() - 1;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            references.push_back(colorAttachmentRef);

            if (resolve) {
                VkAttachmentDescription colorAttachmentResolve{};
                colorAttachmentResolve.format = format;
                colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
                colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachmentResolve.finalLayout = present
                                                         ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                                                         : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                attachments.push_back(colorAttachmentResolve);

                VkAttachmentReference colorAttachmentResolveRef{};
                colorAttachmentResolveRef.attachment = attachments.size() - 1;
                colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                resolveReferences.push_back(colorAttachmentResolveRef);
            }
        }

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = references.size();
        subpass.pColorAttachments = references.data();
        subpass.pResolveAttachments = resolve ? resolveReferences.data() : nullptr;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = attachments.size();
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        if (depth) {
            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = depthFormat;
            depthAttachment.samples = depthSamples;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            attachments.push_back(depthAttachment);
            subpass.pDepthStencilAttachment = &depthAttachmentRef;
        }

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkSubpassDependency depthDependency{};
        depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        depthDependency.dstSubpass = 0;
        depthDependency.srcStageMask =
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthDependency.srcAccessMask = 0;
        depthDependency.dstStageMask =
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkSubpassDependency dependencies[] = {dependency, depthDependency};
        renderPassInfo.dependencyCount = 2;
        renderPassInfo.pDependencies = dependencies;

        if (vkCreateRenderPass(
                _vkApplication->getDevice()->getRaw(),
                &renderPassInfo,
                nullptr,
                &_raw
            ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass!");
        }
    }

    VKRenderPass::VKRenderPass(Application* application, VkRenderPass pass) :
        _vkApplication(dynamic_cast<AbstractVKApplication*>(
            application->getImplementation())),
        _raw(pass),
        _external(true) {}

    VKRenderPass::~VKRenderPass() {
        if (_raw != VK_NULL_HANDLE && !_external) {
            vkDestroyRenderPass(_vkApplication->getDevice()->getRaw(), _raw, nullptr);
        }
    }

    VkRenderPass VKRenderPass::getRaw() const {
        return _raw;
    }
}
